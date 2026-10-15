#ifndef HAL_QMC5883L_QMC5883L_INT_H_
#define HAL_QMC5883L_QMC5883L_INT_H_

typedef struct {
    s16 Mag_X;
    s16 Mag_Y;
    s16 Mag_Z;
} QMC5883L_Data_t;

/* Initializes the QMC5883L for continuous measurement mode */
void HQMC5883L_vInit(void);

/* Reads the X, Y, and Z axes into the provided struct */
void HQMC5883L_vReadData(QMC5883L_Data_t *A_xData);

#endif /* HAL_QMC5883L_QMC5883L_INT_H_ */
