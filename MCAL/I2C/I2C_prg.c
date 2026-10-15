#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"
#include "../GPIO/GPIO_int.h" // Add this to access GPIO drivers
#include "I2C_int.h"
#include "I2C_prv.h"
#include "I2C_cfg.h"

#define I2C_TIMEOUT  10000UL

/* Internal helper to recover the I2C peripheral if a wire glitches */
static void MI2C_vBusRecover(void)
{
    /* 1. Disable I2C Peripheral */
    CLR_BIT(I2C_SELECTED_PERIPHERAL->CR1, I2C_CR1_PE);

    /* 2. Temporarily hijack PB6 (SCL) and PB7 (SDA) as standard outputs */
    MGPIO_vSetMode(GPIO_PORTB, GPIO_PIN6, GPIO_MODE_OUTPUT);
    MGPIO_vSetMode(GPIO_PORTB, GPIO_PIN7, GPIO_MODE_OUTPUT);
    MGPIO_vSetOutputType(GPIO_PORTB, GPIO_PIN6, GPIO_OT_OPENDRAIN);
    MGPIO_vSetOutputType(GPIO_PORTB, GPIO_PIN7, GPIO_OT_OPENDRAIN);

    MGPIO_vSetPinValue(GPIO_PORTB, GPIO_PIN7, GPIO_HIGH); // Release SDA

    /* 3. Send 9 Clock pulses on SCL to trick the stuck sensor into finishing */
    for(int i = 0; i < 9; i++)
    {
        MGPIO_vSetPinValue(GPIO_PORTB, GPIO_PIN6, GPIO_LOW); // SCL Low
        for(volatile int j = 0; j < 1000; j++);
        MGPIO_vSetPinValue(GPIO_PORTB, GPIO_PIN6, GPIO_HIGH); // SCL High
        for(volatile int j = 0; j < 1000; j++);
    }

    /* 4. Generate a manual I2C STOP condition */
    MGPIO_vSetPinValue(GPIO_PORTB, GPIO_PIN6, GPIO_LOW);
    MGPIO_vSetPinValue(GPIO_PORTB, GPIO_PIN7, GPIO_LOW);
    for(volatile int j = 0; j < 1000; j++);
    MGPIO_vSetPinValue(GPIO_PORTB, GPIO_PIN6, GPIO_HIGH);
    for(volatile int j = 0; j < 1000; j++);
    MGPIO_vSetPinValue(GPIO_PORTB, GPIO_PIN7, GPIO_HIGH);

    /* 5. Return pins to Alternate Function (AF4 for I2C1) */
    MGPIO_vSetMode(GPIO_PORTB, GPIO_PIN6, GPIO_MODE_ALF);
    MGPIO_vSetMode(GPIO_PORTB, GPIO_PIN7, GPIO_MODE_ALF);

    /* 6. Hardware reset the I2C silicon to clear BUSY flags */
    SET_BIT(I2C_SELECTED_PERIPHERAL->CR1, I2C_CR1_SWRST);
    for(volatile u32 i = 0; i < 1000; i++);
    CLR_BIT(I2C_SELECTED_PERIPHERAL->CR1, I2C_CR1_SWRST);

    /* 7. Re-initialize the I2C registers */
    MI2C_vInit();
}
void MI2C_vInit(void)
{
    /* 1. Disable the I2C peripheral */
    CLR_BIT(I2C_SELECTED_PERIPHERAL->CR1, I2C_CR1_PE);

    /* 2. Force a Software Reset (Clears locked BUSY flag after pressing Reset button) */
    SET_BIT(I2C_SELECTED_PERIPHERAL->CR1, I2C_CR1_SWRST);
    for(volatile u32 i = 0; i < 1000; i++); // Short delay
    CLR_BIT(I2C_SELECTED_PERIPHERAL->CR1, I2C_CR1_SWRST);

    /* 3. Set the APB1 clock frequency in CR2 */
    I2C_SELECTED_PERIPHERAL->CR2 &= ~(0x3F);
    I2C_SELECTED_PERIPHERAL->CR2 |= (I2C_APB1_FREQ_MHZ & 0x3F);

    /* 4. Configure Clock Control Register (CCR) for 100kHz Standard Mode */
    u32 L_u32CCRValue = (I2C_APB1_FREQ_MHZ * 1000000) / (I2C_CLOCK_SPEED * 2);
    I2C_SELECTED_PERIPHERAL->CCR &= ~(0xFFF);
    I2C_SELECTED_PERIPHERAL->CCR |= (L_u32CCRValue & 0xFFF);

    /* 5. Configure Maximum Rise Time (TRISE) */
    I2C_SELECTED_PERIPHERAL->TRISE = I2C_APB1_FREQ_MHZ + 1;

    /* 6. Enable the I2C peripheral */
    SET_BIT(I2C_SELECTED_PERIPHERAL->CR1, I2C_CR1_PE);
}

void MI2C_vSendStart(void)
{
    u32 L_u32Timeout = I2C_TIMEOUT;
    SET_BIT(I2C_SELECTED_PERIPHERAL->CR1, I2C_CR1_START);

    while(!GET_BIT(I2C_SELECTED_PERIPHERAL->SR1, I2C_SR1_SB))
    {
        if(--L_u32Timeout == 0) { MI2C_vBusRecover(); return; }
    }
}

void MI2C_vSendStop(void)
{
    SET_BIT(I2C_SELECTED_PERIPHERAL->CR1, I2C_CR1_STOP);
}

void MI2C_vSendSlaveAddress(u8 A_u8SlaveAddress, u8 A_u8ReadWrite)
{
    u32 L_u32Timeout = I2C_TIMEOUT;
    u8 L_u8AddressByte = (A_u8SlaveAddress << 1) | A_u8ReadWrite;

    I2C_SELECTED_PERIPHERAL->DR = L_u8AddressByte;

    while(!GET_BIT(I2C_SELECTED_PERIPHERAL->SR1, I2C_SR1_ADDR))
    {
        /* Also abort immediately if the sensor sends a NACK (AF bit 10) */
        if(GET_BIT(I2C_SELECTED_PERIPHERAL->SR1, I2C_SR1_AF) || (--L_u32Timeout == 0))
        {
            CLR_BIT(I2C_SELECTED_PERIPHERAL->SR1, I2C_SR1_AF);
            MI2C_vBusRecover();
            return;
        }
    }

    /* Clear ADDR flag by reading SR1 then SR2 */
    volatile u32 L_u32DummyRead;
    L_u32DummyRead = I2C_SELECTED_PERIPHERAL->SR1;
    L_u32DummyRead = I2C_SELECTED_PERIPHERAL->SR2;
    (void)L_u32DummyRead;
}

void MI2C_vTransmitByte(u8 A_u8Data)
{
    u32 L_u32Timeout = I2C_TIMEOUT;
    while(!GET_BIT(I2C_SELECTED_PERIPHERAL->SR1, I2C_SR1_TXE))
    {
        if(--L_u32Timeout == 0) { MI2C_vBusRecover(); return; }
    }

    I2C_SELECTED_PERIPHERAL->DR = A_u8Data;

    L_u32Timeout = I2C_TIMEOUT;
    while(!GET_BIT(I2C_SELECTED_PERIPHERAL->SR1, I2C_SR1_BTF))
    {
        if(--L_u32Timeout == 0) { MI2C_vBusRecover(); return; }
    }
}

u8 MI2C_u8ReceiveByteWithAck(void)
{
    u32 L_u32Timeout = I2C_TIMEOUT;
    SET_BIT(I2C_SELECTED_PERIPHERAL->CR1, I2C_CR1_ACK);

    while(!GET_BIT(I2C_SELECTED_PERIPHERAL->SR1, I2C_SR1_RXNE))
    {
        if(--L_u32Timeout == 0) { MI2C_vBusRecover(); return 0; }
    }

    return (u8)(I2C_SELECTED_PERIPHERAL->DR);
}

u8 MI2C_u8ReceiveByteWithNack(void)
{
    u32 L_u32Timeout = I2C_TIMEOUT;
    CLR_BIT(I2C_SELECTED_PERIPHERAL->CR1, I2C_CR1_ACK);

    while(!GET_BIT(I2C_SELECTED_PERIPHERAL->SR1, I2C_SR1_RXNE))
    {
        if(--L_u32Timeout == 0) { MI2C_vBusRecover(); return 0; }
    }

    return (u8)(I2C_SELECTED_PERIPHERAL->DR);
}
