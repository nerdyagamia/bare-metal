
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"

#include "EXTI_int.h"
#include "EXTI_prv.h"
//array of 16 pointers , all 16 pointing to address 0
static void (*G_Fptr[16])(void) = {NULL} ;
/*
 * EXTI PR register has a specific hardware property called rc_w1
 * writing 0xFFFFFFFF at startup, Clears every single pending interrupt flag before we start the system
 * This prevents a scenario where a noisy voltage spike happened during boot-up
 * would cause immediately trigger an interrupt the moment you enable the NVIC
 *
 */
void MEXTI_vInit(void)
{
	// CLEAR FLAG
	EXTI->PR = 0xFFFFFFFF;
}

void MEXTI_vEnableINT(u8 A_u8LineNo)
{
	/*
	 * The STM32F401 has EXTI lines up to 22
	 *lines 0 through 15 are explicitly mapped to the physical GPIO pins -> PA0-PA15
	 * By restricting A_u8LineNo to less than 16,
	 *  the software guarantees we are only unmasking physical external pin interrupts
	 *   and preventing out-of-bounds bit shifts
	 *
	 */
	if(A_u8LineNo <16)
	{
		SET_BIT(EXTI->IMR, A_u8LineNo);
	}
}

void MEXTI_vDisableINT(u8 A_u8LineNo)
{
	if(A_u8LineNo <16)
	{
		CLR_BIT(EXTI->IMR, A_u8LineNo);
	}
}

void MEXTI_vSetTrigger(u8 A_u8LineNo, u8 A_u8Trigger_t)
{
	if(A_u8LineNo <16)
		{
			switch(A_u8Trigger_t)
			{
			case EXTI_RISING_EDGE:
				SET_BIT(EXTI->RTSR, A_u8LineNo );
				CLR_BIT(EXTI->FTSR, A_u8LineNo );
				break;

			case EXTI_FALLING_EDGE:
				CLR_BIT(EXTI->RTSR, A_u8LineNo );
				SET_BIT(EXTI->FTSR, A_u8LineNo );
				break;

			case EXTI_ONCHANGE_EDGE:
				SET_BIT(EXTI->RTSR, A_u8LineNo );
				SET_BIT(EXTI->FTSR, A_u8LineNo );
				break;
			}
		}
}

void MEXTI_vSoftwareINT (u8 A_u8LineNo)
{
	if(A_u8LineNo <16)
		{
			SET_BIT(EXTI->SWIER ,A_u8LineNo);
		}
}


void MEXTI_vSetCallBack(void(*A_xFptr)(void), u8 A_u8LineNo)
{
	if(A_u8LineNo <16)
			{
	// POINTER -> FUNCTION I WANT TO EXECUTE IN EXTIx 0.1.2.3
	G_Fptr[A_u8LineNo] = A_xFptr;
			}
}

//ISR
/*
 * ARM GCC is programmed to look for the exact literal string EXTI0_IRQHandler
 * When it sees this name, the compiler automatically places the memory address of this function
 * into the correct slot in the Vector Table
 * Because this function interacts directly with the EXTI Pending Register (PR) to clear the hardware flag,
 * it must live in the MCAL layer
 */
void EXTI0_IRQHandler(void)
{
	if(G_Fptr[EXTI_LINE0] != NULL)
	{
	//to the end of the pointer
	//Take the memory address written on 0, go there, and execute the code.
	G_Fptr[EXTI_LINE0]();
	}
	//we clear the physical hardware flag
	SET_BIT(EXTI->PR , EXTI_LINE0);
}
/*
 *
 */

void EXTI1_IRQHandler(void)
{
	if(G_Fptr[EXTI_LINE1] != NULL)
	{
	G_Fptr[EXTI_LINE1]();
	}
	SET_BIT(EXTI->PR , EXTI_LINE1);
}

void EXTI2_IRQHandler(void)
{
	if(G_Fptr[EXTI_LINE2] != NULL)
	{
	G_Fptr[EXTI_LINE2]();
	}
	SET_BIT(EXTI->PR , EXTI_LINE2);
}

void EXTI3_IRQHandler(void)
{
	if(G_Fptr[EXTI_LINE3] != NULL)
	{
	G_Fptr[EXTI_LINE3]();
	}
	SET_BIT(EXTI->PR , EXTI_LINE3);
}

void EXTI4_IRQHandler(void)
{
	if(G_Fptr[EXTI_LINE4] != NULL)
	{
	G_Fptr[EXTI_LINE4]();
	}
	SET_BIT(EXTI->PR , EXTI_LINE4);
}

void EXTI9_5_IRQHandler(void)
{
	if(GET_BIT(EXTI->PR, EXTI_LINE5)) {
			if(G_Fptr[EXTI_LINE5] != NULL) { G_Fptr[EXTI_LINE5](); }
			SET_BIT(EXTI->PR , EXTI_LINE5);
		}
		if(GET_BIT(EXTI->PR, EXTI_LINE6)) {
			if(G_Fptr[EXTI_LINE6] != NULL) { G_Fptr[EXTI_LINE6](); }
			SET_BIT(EXTI->PR , EXTI_LINE6);
		}
		if(GET_BIT(EXTI->PR, EXTI_LINE7)) {
			if(G_Fptr[EXTI_LINE7] != NULL) { G_Fptr[EXTI_LINE7](); }
			SET_BIT(EXTI->PR , EXTI_LINE7);
		}
		if(GET_BIT(EXTI->PR, EXTI_LINE8)) {
			if(G_Fptr[EXTI_LINE8] != NULL) { G_Fptr[EXTI_LINE8](); }
			SET_BIT(EXTI->PR , EXTI_LINE8);
		}
		if(GET_BIT(EXTI->PR, EXTI_LINE9)) {
			if(G_Fptr[EXTI_LINE9] != NULL) { G_Fptr[EXTI_LINE9](); }
			SET_BIT(EXTI->PR , EXTI_LINE9);
		}
}
