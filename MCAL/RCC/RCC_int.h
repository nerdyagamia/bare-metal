#ifndef RCC_INT_H_
#define RCC_INT_H_

//buses
#define	RCC_AHB1	0
#define	RCC_AHB2	1
#define	RCC_APB1	2
#define	RCC_APB2	3

// Per
#define RCC_GPIOA	0
#define RCC_GPIOB	1




void MRCC_vInit(void);

void MRCC_vEnableCLK(u8 A_u8BusID, u8 A_u8PeripheralID);

void MRCC_vDisableCLK(u8 A_u8BusID, u8 A_u8PeripheralID);
// Adds compatibility for the TIM driver
Std_ReturnType RCC_GetFrequency(u32 *Copy_pu32Frequency);


#endif /* RCC_INT_H_ */

