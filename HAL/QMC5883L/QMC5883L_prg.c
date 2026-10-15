#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"

#include "../../MCAL/I2C/I2C_int.h"
#include "../../MCAL/SYSTICK/SYSTICK_int.h"

#include "QMC5883L_int.h"

#define QMC5883L_ADDR           0x0D
#define QMC5883L_DATA_X_LSB     0x00
#define QMC5883L_CONTROL_1      0x09
#define QMC5883L_SET_RESET      0x0B

void HQMC5883L_vInit(void)
{
    /*
     * 1. Write to the Set/Reset Period Register (0x0B)
     * The datasheet recommends writing 0x01 to this register.
     */
    MI2C_vSendStart();
    MI2C_vSendSlaveAddress(QMC5883L_ADDR, I2C_WRITE);
    MI2C_vTransmitByte(QMC5883L_SET_RESET);
    MI2C_vTransmitByte(0x01);
    MI2C_vSendStop();

    MSYSTICK_vSetDelay_ms(10);

    /*
     * 2. Write to Control Register 1 (0x09)
     * Bit configuration:
     * OSR (Over Sample Ratio) = 512 (0b00)
     * RNG (Range) = 8G (0b01)
     * ODR (Output Data Rate) = 200Hz (0b11)
     * MODE = Continuous (0b01)
     * Total Byte: 0b00011101 = 0x1D
     */
    MI2C_vSendStart();
    MI2C_vSendSlaveAddress(QMC5883L_ADDR, I2C_WRITE);
    MI2C_vTransmitByte(QMC5883L_CONTROL_1);
    MI2C_vTransmitByte(0x1D);
    MI2C_vSendStop();

    /* Give it time to stabilize after mode switch */
    MSYSTICK_vSetDelay_ms(10);
}

void HQMC5883L_vReadData(QMC5883L_Data_t *A_xData)
{
    u8 L_u8RecData[6];

    /* 1. Set the register pointer to the start of the data (0x00) */
    MI2C_vSendStart();
    MI2C_vSendSlaveAddress(QMC5883L_ADDR, I2C_WRITE);
    MI2C_vTransmitByte(QMC5883L_DATA_X_LSB);

    /* 2. Generate Repeated Start to switch to Read Mode */
    MI2C_vSendStart();
    MI2C_vSendSlaveAddress(QMC5883L_ADDR, I2C_READ);

    /* 3. Burst read the first 5 bytes, sending an ACK after each */
    for(u8 i = 0; i < 5; i++)
    {
        L_u8RecData[i] = MI2C_u8ReceiveByteWithAck();
    }

    /* 4. Read the 6th byte and send a NACK to terminate, followed by STOP */
    L_u8RecData[5] = MI2C_u8ReceiveByteWithNack();
    MI2C_vSendStop();

    /*
     * 5. Reconstruct the 16-bit signed values.
     * Note: QMC5883L sends LSB first, then MSB!
     */
    A_xData->Mag_X = (s16)((L_u8RecData[1] << 8) | L_u8RecData[0]);
    A_xData->Mag_Y = (s16)((L_u8RecData[3] << 8) | L_u8RecData[2]);
    A_xData->Mag_Z = (s16)((L_u8RecData[5] << 8) | L_u8RecData[4]);
}
