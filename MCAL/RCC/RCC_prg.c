
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"

#include "RCC_int.h"
#include "RCC_prv.h"
#include "RCC_cfg.h"


void MRCC_vInit(void)
{
#if RCC_SYS_CLK == HSE_CLK

#if	HSE_BYPASS	== RC_CLK

	SET_BIT(RCC->CR,HSEBYP);

#elif HSE_BYPASS == MECHANICAL_CLK

	CLR_BIT(RCC->CR,HSEBYP);

#endif

	//HSE on
	SET_BIT(RCC->CR,16);

	while(!GET_BIT(RCC->CR, 17));

	CLR_BIT(RCC->CFGR, 1);
	SET_BIT(RCC->CFGR, 0);

#elif	RCC_SYS_CLK == HSI_CLK

	SET_BIT(RCC->CR, 0);

	while(!GET_BIT(RCC->CR, 1));

	CLR_BIT(RCC->CFGR, 1);
	CLR_BIT(RCC->CFGR, 0);


#elif	RCC_SYS_CLK == PLL_CLK

#else
#error "invalid option"
#endif

}



void MRCC_vEnableCLK(u8 A_u8BusID, u8 A_u8PeripheralID)
{
	switch(A_u8BusID)
	{

	case RCC_AHB1:
		SET_BIT(RCC->AHB1ENR, A_u8PeripheralID);
		break;

	case RCC_AHB2:
		SET_BIT(RCC->AHB2ENR, A_u8PeripheralID);

		break;

	case RCC_APB1:
		SET_BIT(RCC->APB1ENR, A_u8PeripheralID);

			break;

	case RCC_APB2:
		SET_BIT(RCC->APB2ENR, A_u8PeripheralID);

				break;
	}
}

void MRCC_vDisableCLK(u8 A_u8BusID, u8 A_u8PeripheralID)
{
	switch(A_u8BusID)
	{

	case RCC_AHB1:
		CLR_BIT(RCC->AHB1ENR, A_u8PeripheralID);
		break;

	case RCC_AHB2:
		CLR_BIT(RCC->AHB2ENR, A_u8PeripheralID);

		break;

	case RCC_APB1:
		CLR_BIT(RCC->APB1ENR, A_u8PeripheralID);

			break;

	case RCC_APB2:
		CLR_BIT(RCC->APB2ENR, A_u8PeripheralID);

				break;
	}
}

Std_ReturnType RCC_GetFrequency(u32 *Copy_pu32Frequency)
{
    if (Copy_pu32Frequency == NULL)
    {
        return E_NOT_OK;
    }

    u32 Local_u32SystemClock = 0;

    // Read the System Clock Switch Status (SWS) bits 2 and 3 from your RCC CFGR register
    u32 Local_u32ClockSource = (RCC->CFGR >> 2) & 0x03;

    switch (Local_u32ClockSource)
    {
        case 0x00: // HSI selected
            Local_u32SystemClock = 16000000UL; // 16 MHz internal clock
            break;

        case 0x01: // HSE selected
            Local_u32SystemClock = 25000000UL; // 25 MHz external crystal (Change to 8000000UL if you use an 8MHz crystal)
            break;

        case 0x02: // PLL selected
            // For a quick fix, hardcode your target PLL frequency here (e.g., 84 MHz for STM32F401)
            Local_u32SystemClock = 84000000UL;
            break;

        default:
            return E_NOT_OK;
    }

    *Copy_pu32Frequency = Local_u32SystemClock;
    return E_OK;
}
