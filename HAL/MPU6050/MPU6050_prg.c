#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"

#include "../../MCAL/I2C/I2C_int.h"
#include "../../MCAL/SYSTICK/SYSTICK_int.h"

#include "MPU6050_int.h"

#define MPU6050_ADDR            0x68
#define MPU6050_PWR_MGMT_1      0x6B
#define MPU6050_ACCEL_XOUT_H    0x3B

void HMPU6050_vInit(void)
{
    /* 1. Wake up the MPU-6050 (Write 0x00 to PWR_MGMT_1) */
    MI2C_vSendStart();
    MI2C_vSendSlaveAddress(MPU6050_ADDR, I2C_WRITE);
    MI2C_vTransmitByte(MPU6050_PWR_MGMT_1);
    MI2C_vTransmitByte(0x00);
    MI2C_vSendStop();

    /* Give the sensor a few milliseconds to stabilize its PLL */
    MSYSTICK_vSetDelay_ms(10);

    /*
     * Note: By default, the MPU6050 boots up with:
     * Accel Range = +/- 2g
     * Gyro Range = +/- 250 deg/sec
     * These are perfectly fine for a standard Madgwick filter setup.
     */
}

void HMPU6050_vReadData(MPU6050_Data_t *A_xData)
{
    u8 L_u8RecData[14];

    /* 1. Set the register pointer to the start of the data (ACCEL_XOUT_H) */
    MI2C_vSendStart();
    MI2C_vSendSlaveAddress(MPU6050_ADDR, I2C_WRITE);
    MI2C_vTransmitByte(MPU6050_ACCEL_XOUT_H);

    /* 2. Generate Repeated Start to switch the bus to Read Mode */
    MI2C_vSendStart();
    MI2C_vSendSlaveAddress(MPU6050_ADDR, I2C_READ);

    /* 3. Burst read the first 13 bytes, sending an ACK after each */
    for(u8 i = 0; i < 13; i++)
    {
        L_u8RecData[i] = MI2C_u8ReceiveByteWithAck();
    }

    /* 4. Read the 14th byte (Gyro_Z_L) and send a NACK to terminate, followed by STOP */
    L_u8RecData[13] = MI2C_u8ReceiveByteWithNack();
    MI2C_vSendStop();

    /* 5. Reconstruct the 16-bit signed values (High byte << 8 | Low byte) */
    A_xData->Accel_X = (s16)((L_u8RecData[0] << 8) | L_u8RecData[1]);
    A_xData->Accel_Y = (s16)((L_u8RecData[2] << 8) | L_u8RecData[3]);
    A_xData->Accel_Z = (s16)((L_u8RecData[4] << 8) | L_u8RecData[5]);

    /* Skip L_u8RecData[6] and [7] (Temperature Data) */

    A_xData->Gyro_X  = (s16)((L_u8RecData[8] << 8) | L_u8RecData[9]);
    A_xData->Gyro_Y  = (s16)((L_u8RecData[10] << 8) | L_u8RecData[11]);
    A_xData->Gyro_Z  = (s16)((L_u8RecData[12] << 8) | L_u8RecData[13]);
}
