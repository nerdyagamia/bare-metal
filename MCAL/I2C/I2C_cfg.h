#ifndef MCAL_I2C_I2C_CFG_H_
#define MCAL_I2C_I2C_CFG_H_

/*
 * Define the APB1 clock frequency in MHz.
 * If your SYSCLK is 84 MHz and APB1 prescaler is /2, APB1 is 42 MHz.
 * If you are using HSI (16 MHz) with no prescaler, this is 16.
 */
#define I2C_APB1_FREQ_MHZ       16

/*
 * I2C Standard Mode Clock Speed (100 kHz is safest for breadboard/jumper wires)
 * Max is 100000 (100 kHz) for Standard Mode (SM).
 */
#define I2C_CLOCK_SPEED         100000U

/* Select which I2C peripheral to use (I2C1 is PB6/PB7 or PB8/PB9) */
#define I2C_SELECTED_PERIPHERAL I2C1

#endif /* MCAL_I2C_I2C_CFG_H_ */
