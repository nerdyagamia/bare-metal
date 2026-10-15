#include <stdio.h> // Needed for sprintf
#include "../LIB/STD_TYPES.h"
#include "../LIB/BIT_MATH.h"

/* MCAL Includes */
#include "../MCAL/RCC/RCC_int.h"
#include "../MCAL/GPIO/GPIO_int.h"
#include "../MCAL/SYSTICK/SYSTICK_int.h"
#include "../MCAL/I2C/I2C_int.h"
#include "../MCAL/SPI/SPI_int.h"

/* HAL Includes */
#include "../HAL/TFT/TFT_int.h"
#include "../HAL/MPU6050/MPU6050_int.h"
#include "../HAL/QMC5883L/QMC5883L_int.h"

#include "Madgwick.h"


/* Common RGB565 Colors */
#define TFT_BLACK   0x0000
#define TFT_WHITE   0xFFFF
#define TFT_GREEN   0x07E0
#define TFT_RED     0xF800
#define TFT_BLUE    0x001F

/* Madgwick Globals */
extern void MadgwickAHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);
extern float Roll, Pitch, Yaw;

void System_vHardwareSetup(void)
{
    MRCC_vInit();

    MRCC_vEnableCLK(RCC_AHB1, RCC_GPIOA);
    MRCC_vEnableCLK(RCC_AHB1, RCC_GPIOB);
    MRCC_vEnableCLK(RCC_APB2, 12); // SPI1
    MRCC_vEnableCLK(RCC_APB1, 21); // I2C1

    GPIOx_PinConfig_t I2C1_SCL = { .Port = GPIO_PORTB, .Pin = GPIO_PIN6, .Mode = GPIO_MODE_ALF,
                                   .Speed = GPIO_SPEED_HIGH, .OutputType = GPIO_OT_OPENDRAIN,
                                   .PullType = GPIO_PULL_UP, .AltFunc = GPIO_AF4 };
    GPIOx_PinConfig_t I2C1_SDA = { .Port = GPIO_PORTB, .Pin = GPIO_PIN7, .Mode = GPIO_MODE_ALF,
                                   .Speed = GPIO_SPEED_HIGH, .OutputType = GPIO_OT_OPENDRAIN,
                                   .PullType = GPIO_PULL_UP, .AltFunc = GPIO_AF4 };
    MGPIO_vPinInit(&I2C1_SCL);
    MGPIO_vPinInit(&I2C1_SDA);

    GPIOx_PinConfig_t SPI1_SCK  = { .Port = GPIO_PORTA, .Pin = GPIO_PIN5, .Mode = GPIO_MODE_ALF,
                                    .Speed = GPIO_SPEED_HIGH, .OutputType = GPIO_OT_PUSHPULL,
                                    .PullType = GPIO_NO_PULL, .AltFunc = GPIO_AF5 };
    GPIOx_PinConfig_t SPI1_MOSI = { .Port = GPIO_PORTA, .Pin = GPIO_PIN7, .Mode = GPIO_MODE_ALF,
                                    .Speed = GPIO_SPEED_HIGH, .OutputType = GPIO_OT_PUSHPULL,
                                    .PullType = GPIO_NO_PULL, .AltFunc = GPIO_AF5 };
    MGPIO_vPinInit(&SPI1_SCK);
    MGPIO_vPinInit(&SPI1_MOSI);

    MI2C_vInit();
    HTFT_vInit();
    HMPU6050_vInit();
    HQMC5883L_vInit();

    HTFT_vFillBackgroundColor(TFT_BLACK);
}
/* Helper function to format floats using integer sprintf */
void FormatAngle(char* buffer, const char* name, float angle)
{
    /* Extract the sign */
    char sign = (angle < 0) ? '-' : ' ';

    /* Get absolute value */
    float abs_angle = (angle < 0) ? -angle : angle;

    /* Extract integer and fractional parts */
    int int_part = (int)abs_angle;
    int frac_part = (int)((abs_angle - int_part) * 100); // 2 decimal places

    /* Print using standard integer formatting */
    sprintf(buffer, "%s %c%d.%02d deg", name, sign, int_part, frac_part);
}

int main(void)
{
    System_vHardwareSetup();

    MPU6050_Data_t  MPU_Data;
    QMC5883L_Data_t QMC_Data;

    /* Buffers for text rendering */
    char str_roll[25];
    char str_pitch[25];
    char str_yaw[25];

    /* Print Static Header */
    HTFT_vPrintString(5, 5, "9-DOF AHRS Status:", TFT_GREEN, TFT_BLACK);

    u8 display_counter = 0;

    while(1)
    {
        /* 1. Read Raw Sensors (~2ms over I2C) */
        HMPU6050_vReadData(&MPU_Data);
        HQMC5883L_vReadData(&QMC_Data);

        /* 2. Convert MPU-6050 to physical units (matches Arduino code) */
        float ax = (float)MPU_Data.Accel_X / 16384.0f;
        float ay = (float)MPU_Data.Accel_Y / 16384.0f;
        float az = (float)MPU_Data.Accel_Z / 16384.0f;

        float gx = (float)MPU_Data.Gyro_X / 131.0f;
        float gy = (float)MPU_Data.Gyro_Y / 131.0f;
        float gz = (float)MPU_Data.Gyro_Z / 131.0f;

        /* 3. Pass raw QMC5883L values (matches QMC5883LCompass.getX/Y/Z) */
        float mx = (float)QMC_Data.Mag_X;
        float my = (float)QMC_Data.Mag_Y;
        float mz = (float)QMC_Data.Mag_Z;

        /* 4. Update Madgwick Filter at 100Hz */
        MadgwickAHRSupdate(gx, gy, gz, ax, ay, az, mx, my, mz);

        /* 5. Update TFT screen every 10th loop (10Hz) so SPI doesn't stall the 100Hz filter */
        display_counter++;
        if (display_counter >= 10)
        {
            FormatAngle(str_roll,  "Roll: ", Roll);
            FormatAngle(str_pitch, "Pitch:", Pitch);
            FormatAngle(str_yaw,   "Yaw:  ", Yaw);

            HTFT_vPrintString(5, 25, str_roll, TFT_WHITE, TFT_BLACK);
            HTFT_vPrintString(5, 45, str_pitch, TFT_WHITE, TFT_BLACK);
            HTFT_vPrintString(5, 65, str_yaw, TFT_WHITE, TFT_BLACK);

            display_counter = 0;
        }
        else
        {
            /* Wait ~8ms on non-display loops (8ms delay + ~2ms I2C = 10ms / 100Hz) */
            MSYSTICK_vSetDelay_ms(8);
        }
    }

    return 0;
}
