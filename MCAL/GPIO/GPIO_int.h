#ifndef MCAL_GPIO_GPIO_INT_H_
#define MCAL_GPIO_GPIO_INT_H_

// ports
#define	GPIO_PORTA	0
#define	GPIO_PORTB	1
#define	GPIO_PORTC	2

//PINS
#define	GPIO_PIN0	0
#define	GPIO_PIN1	1
#define	GPIO_PIN2	2
#define	GPIO_PIN3	3
#define	GPIO_PIN4	4
#define	GPIO_PIN5	5
#define	GPIO_PIN6	6
#define	GPIO_PIN7	7
#define	GPIO_PIN8	8
#define	GPIO_PIN9	9
#define	GPIO_PIN10	10
#define	GPIO_PIN11	11
#define	GPIO_PIN12	12
#define	GPIO_PIN13	13
#define	GPIO_PIN14	14
#define	GPIO_PIN15	15
//ALTERNATE
#define GPIO_AF0  0b0000
#define GPIO_AF1  0b0001
#define GPIO_AF2  0b0010
#define GPIO_AF3  0b0011
#define GPIO_AF4  0b0100
#define GPIO_AF5  0b0101
#define GPIO_AF6  0b0110
#define GPIO_AF7  0b0111
#define GPIO_AF8  0b1000
#define GPIO_AF9  0b1001
#define GPIO_AF10 0b1010
#define GPIO_AF11 0b1011
#define GPIO_AF12 0b1100
#define GPIO_AF13 0b1101
#define GPIO_AF14 0b1110
#define GPIO_AF15 0b1111


// MODE
#define GPIO_MODE_INPUT		0b00
#define GPIO_MODE_OUTPUT	0b01
#define GPIO_MODE_ALF		0b10
#define GPIO_MODE_ANALOG	0b11

//OUTPUT TYPES
#define GPIO_OT_PUSHPULL	0
#define GPIO_OT_OPENDRAIN	1

//SPEED
#define	GPIO_SPEED_LOW		0b00
#define	GPIO_SPEED_MED		0b01
#define	GPIO_SPEED_HIGH		0b10
#define	GPIO_SPEED_VHIGH	0b11

//PULLTYPE
#define GPIO_NO_PULL	0b00
#define GPIO_PULL_UP	0b01
#define GPIO_PULL_DOWN	0b10

//PIN VALUE
#define GPIO_HIGH	1
#define	GPIO_LOW	0

typedef struct{
	u8 Port;
	u8 Pin;
	u8 Mode;
	u8 OutputType;
	u8 Speed;
	u8 PullType;
	u8 AltFunc;

}GPIOx_PinConfig_t;

void MGPIO_vPinInit(GPIOx_PinConfig_t* A_xPinCfg);

void MGPIO_vSetMode(u8 A_u8PortID, u8 A_u8PinNum, u8 A_u8Mode);

void MGPIO_vSetOutputType(u8 A_u8PortID, u8 A_u8PinNum, u8 A_u8OutputType);

void MGPIO_vSetOutputSpeed(u8 A_u8PortID, u8 A_u8PinNum, u8 A_u8OutputSpeed);

void MGPIO_vSetPullType(u8 A_u8PortID, u8 A_u8PinNum, u8 A_u8PullType);

void MGPIO_vSetAlt(u8 A_u8PortID, u8 A_u8PinNum, u8 A_u8AFx);




void MGPIO_vSetPinValue(u8 A_u8PortID, u8 A_u8PinNum, u8 A_u8PinValue);

void MGPIO_vSetPinValueAtomic(u8 A_u8PortID, u8 A_u8PinNum, u8 A_u8PinValue);

void MGPIO_vTogPinValue(u8 A_u8PortID, u8 A_u8PinNum);

u8 MGPIO_u8GetPinValue(u8 A_u8PortID, u8 A_u8PinNum);



#endif /* MCAL_GPIO_GPIO_INT_H_ */
