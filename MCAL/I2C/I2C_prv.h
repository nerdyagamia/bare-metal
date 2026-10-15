#ifndef MCAL_I2C_I2C_PRV_H_
#define MCAL_I2C_I2C_PRV_H_

#define I2C1_BASE_ADDR  0x40005400U
#define I2C2_BASE_ADDR  0x40005800U
#define I2C3_BASE_ADDR  0x40005C00U

typedef struct {
    volatile u32 CR1;
    volatile u32 CR2;
    volatile u32 OAR1;
    volatile u32 OAR2;
    volatile u32 DR;
    volatile u32 SR1;
    volatile u32 SR2;
    volatile u32 CCR;
    volatile u32 TRISE;
    volatile u32 FLTR;
} I2C_MemMap_t;

#define I2C1  ((volatile I2C_MemMap_t*)(I2C1_BASE_ADDR))
#define I2C2  ((volatile I2C_MemMap_t*)(I2C2_BASE_ADDR))
#define I2C3  ((volatile I2C_MemMap_t*)(I2C3_BASE_ADDR))

/* CR1 Bits */
#define I2C_CR1_PE      0
#define I2C_CR1_START   8
#define I2C_CR1_STOP    9
#define I2C_CR1_ACK     10
#define I2C_CR1_SWRST   15

/* SR1 Bits */
#define I2C_SR1_SB      0
#define I2C_SR1_ADDR    1
#define I2C_SR1_BTF     2
#define I2C_SR1_RXNE    6
#define I2C_SR1_TXE     7
#define I2C_SR1_AF      10

/* SR2 Bits */
#define I2C_SR2_MSL     0
#define I2C_SR2_BUSY    1
#define I2C_SR2_TRA     2

#endif /* MCAL_I2C_I2C_PRV_H_ */
