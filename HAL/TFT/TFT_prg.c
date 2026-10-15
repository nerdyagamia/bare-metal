/*
 * TFT_prg.c
 *
 *  Created on: Sep 1, 2026
 *      Author: Hager Adel
 */

#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"

#include "../../MCAL/GPIO/GPIO_int.h"
#include "../../MCAL/SPI/SPI_int.h"
#include "../../MCAL/SYSTICK/SYSTICK_int.h"

#include "TFT_int.h"

static u16 CurrentXStart;
static u16 CurrentXEnd;

static u16 CurrentYStart;
static u16 CurrentYEnd;

GPIOx_PinConfig_t TFT_RST_PIN={
		.Port = GPIO_PORTA,
		.Pin = GPIO_PIN0,
		.Mode = GPIO_MODE_OUTPUT,
		.Speed = GPIO_SPEED_HIGH,
		.OutputType = GPIO_OT_PUSHPULL
};

// SEND DATA 1-> HIGH
// SEND CMD 0 -> LOW
GPIOx_PinConfig_t TFT_A0_PIN={
		.Port = GPIO_PORTA,
		.Pin = GPIO_PIN1,
		.Mode = GPIO_MODE_OUTPUT,
		.Speed = GPIO_SPEED_HIGH,
		.OutputType = GPIO_OT_PUSHPULL
};

static void Reset_Seq(void)
{
    MGPIO_vSetPinValue(TFT_RST_PIN.Port, TFT_RST_PIN.Pin, GPIO_HIGH);
    MSYSTICK_vSetDelay_ms(1); // Ensure HIGH

    MGPIO_vSetPinValue(TFT_RST_PIN.Port, TFT_RST_PIN.Pin, GPIO_LOW);
    MSYSTICK_vSetDelay_ms(2); // Hold LOW for >10us to trigger reset

    MGPIO_vSetPinValue(TFT_RST_PIN.Port, TFT_RST_PIN.Pin, GPIO_HIGH);
    MSYSTICK_vSetDelay_ms(120); // Wait for boot
}

static void Write_cmd(u8 A_u8cmd)
{
	MGPIO_vSetPinValue(TFT_A0_PIN.Port, TFT_A0_PIN.Pin, GPIO_LOW);
	MSPI_u8Transceive(A_u8cmd);
}

static void Write_data(u8 A_u8data)
{
	MGPIO_vSetPinValue(TFT_A0_PIN.Port, TFT_A0_PIN.Pin, GPIO_HIGH);
	MSPI_u8Transceive(A_u8data);
}

void HTFT_vInit(void)
{
    // PIN CONFIG
    MGPIO_vPinInit(&TFT_RST_PIN);
    MGPIO_vPinInit(&TFT_A0_PIN);

    // SPI & SYSTICK
    MSPI_vInit();
    MSYSTIC_Config_t STK_CFG = {
            .InterruptEnable = INT_DISABLE,
            .CLK_SRC = CLK_SRC_AHB_8
    };
    MSYSTICK_vInit(&STK_CFG);

    // Hardware Reset
    Reset_Seq();

    // Software Reset
    Write_cmd(0x01);
    MSYSTICK_vSetDelay_ms(150);

    // Sleep Out
    Write_cmd(0x11);
    MSYSTICK_vSetDelay_ms(150); // MUST be >120ms

    // Color Mode (16-bit RGB565)
    Write_cmd(0x3A);
    Write_data(0x05);

    // Memory Access Control (Orientation)
    Write_cmd(0x36);
    Write_data(0x00);

    // Display On
    Write_cmd(0x29);
}

void HTFT_vShowImage(const u16 A_u16ImgArr[], u16 A_u16ImgSize)
{
	u8 MSB=0;
	u8 LSB=0;
	//set x position
	Write_cmd(0x2A);

	// 0x0000

	//1 send xstart
	//msb
	Write_data(0);
	//least
	Write_data(0);

	// send xEnd
	// 127 = 0x00 7F
	Write_data(0);
	Write_data(127);

	//set y position
	Write_cmd(0x2B);

	// 0x0000

	//1 send ystart
	//msb
	Write_data(0);
	//least
	Write_data(0);

	// send xEnd
	// 159
	Write_data(0);
	Write_data(159);

	//send image
	Write_cmd(0x2C);


	for(u16 i=0; i<A_u16ImgSize ;i++)
	{
		//      0x 1234
		MSB = (A_u16ImgArr[i] & 0xFF00) >>8;
		LSB = A_u16ImgArr[i] & 0x00FF;

		Write_data(MSB);
		Write_data(LSB);

	}


}

void HTFT_vSetXPos(u16 A_u16xStart,u16 A_u16xEnd)
{
	CurrentXStart = A_u16xStart;
	CurrentXEnd = A_u16xEnd;
	//set x position
	Write_cmd(0x2A);

	// 0x0000

	//1 send xstart
	//msb
	Write_data(0);
	//least
	Write_data(A_u16xStart);

	// send xEnd
	// 127 = 0x00 7F
	Write_data(0);
	Write_data(A_u16xEnd);

}
void HTFT_vSetYPos(u16 A_u16YStart,u16 A_u16YEnd)
{

	CurrentYStart = A_u16YStart;
	CurrentYEnd = A_u16YEnd;

	//set y position
	Write_cmd(0x2B);

	// 0x0000

	//1 send ystart
	//msb
	Write_data(0);
	//least
	Write_data(A_u16YStart);

	// send xEnd
	// 159
	Write_data(0);
	Write_data(A_u16YEnd);
}

void HTFT_vFillBackgroundColor(u16 A_u16Color)
{
	HTFT_vSetXPos(0,127);
	HTFT_vSetYPos(0,159);

	HTFT_vFillRectangle(A_u16Color);
}

//0X1234
void HTFT_vFillRectangle(u16 A_u16Color)
{
	u8 MSB =0;
	u8 LSB = 0;
u16 Pixels = (CurrentXEnd-CurrentXStart+1) * (CurrentYEnd - CurrentYStart+1);

Write_cmd(0x2C);

MSB = (A_u16Color & 0xFF00) >>8;
		LSB = A_u16Color & 0x00FF;

for(u16 i=0; i<Pixels ;i++)
{
		Write_data(MSB);
		Write_data(LSB);
}

}

#include "TFT_font.h"

void HTFT_vDrawPixel(u16 x, u16 y, u16 color)
{
    HTFT_vSetXPos(x, x);
    HTFT_vSetYPos(y, y);
    Write_cmd(0x2C);
    Write_data(color >> 8);
    Write_data(color & 0xFF);
}

void HTFT_vPrintChar(u16 x, u16 y, char ch, u16 color, u16 bg_color)
{
    /* Cast to u8 to prevent ARM GCC type-limit warnings on signed chars */
    if((u8)ch < 32 || (u8)ch > 127) return;

    u8 L_u8CharIndex = (u8)ch - 32;

    HTFT_vSetXPos(x, x + 4);
    HTFT_vSetYPos(y, y + 7);
    Write_cmd(0x2C); // RAMWR

    for(u8 row = 0; row < 8; row++)
    {
        for(u8 col = 0; col < 5; col++)
        {
            u8 pixel_data = TFT_Font5x7[L_u8CharIndex][col];

            if(pixel_data & (1 << row))
            {
                Write_data(color >> 8);
                Write_data(color & 0xFF);
            }
            else
            {
                Write_data(bg_color >> 8);
                Write_data(bg_color & 0xFF);
            }
        }
    }
}

void HTFT_vPrintString(u16 x, u16 y, const char *str, u16 color, u16 bg_color)
{
    while(*str)
    {
        HTFT_vPrintChar(x, y, *str, color, bg_color);
        x += 6;

        if(x > 122)
        {
            x = 0;
            y += 10;
        }

        str++;
    }
}
