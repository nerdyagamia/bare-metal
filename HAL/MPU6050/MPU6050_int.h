#ifndef HAL_MPU6050_MPU6050_INT_H_
#define HAL_MPU6050_MPU6050_INT_H_

typedef struct {
    s16 Accel_X;
    s16 Accel_Y;
    s16 Accel_Z;
    s16 Gyro_X;
    s16 Gyro_Y;
    s16 Gyro_Z;
} MPU6050_Data_t;

/* Wake up the sensor and configure default ranges */
void HMPU6050_vInit(void);

/* Reads all 6 axes of data into the provided struct */
void HMPU6050_vReadData(MPU6050_Data_t *A_xData);

#endif /* HAL_MPU6050_MPU6050_INT_H_ */
