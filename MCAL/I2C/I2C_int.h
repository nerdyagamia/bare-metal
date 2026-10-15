#ifndef MCAL_I2C_I2C_INT_H_
#define MCAL_I2C_I2C_INT_H_

/* Read/Write Direction definitions */
#define I2C_WRITE   0
#define I2C_READ    1

/*
 * I2C Initialization
 * Configures the selected I2C peripheral (Clock frequency, Rise time, Control registers)
 */
void MI2C_vInit(void);

/*
 * Generates a START condition on the bus.
 * Also used for Repeated Start (Sr).
 */
void MI2C_vSendStart(void);

/*
 * Generates a STOP condition on the bus.
 */
void MI2C_vSendStop(void);

/*
 * Sends the 7-bit Slave Address along with the Read/Write bit.
 * A_u8SlaveAddress: The 7-bit address of the sensor (e.g., 0x68 for MPU6050)
 * A_u8ReadWrite: I2C_WRITE (0) or I2C_READ (1)
 */
void MI2C_vSendSlaveAddress(u8 A_u8SlaveAddress, u8 A_u8ReadWrite);

/*
 * Transmits a single byte of data over the bus.
 */
void MI2C_vTransmitByte(u8 A_u8Data);

/*
 * Receives a byte of data and responds with an ACK (Acknowledge).
 * Used when reading multiple bytes (like reading X, Y, Z axes sequentially).
 */
u8 MI2C_u8ReceiveByteWithAck(void);

/*
 * Receives a byte of data and responds with a NACK (Not Acknowledge).
 * Used to tell the slave it is the last byte we want to read before sending a STOP.
 */
u8 MI2C_u8ReceiveByteWithNack(void);

#endif /* MCAL_I2C_I2C_INT_H_ */
