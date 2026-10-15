#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"

#include "NVIC_int.h"
#include "NVIC_prv.h"

u8 G_u8GroupPriority=0;

void MNVIC_vEnable_Peripheral_INT(u8 A_u8Position)
{
	/*Each interrupt source has a specific "Position"
	 * for example, EXTI0 is position 6
	 * Register Selection: [A_u8Position / 32]
	 * Bit Selection: A_u8Position % 32
	 * look up the vector table and input the number in the func
	 */
	SET_BIT(NVIC->ISERx[A_u8Position/32] , A_u8Position%32 ) ;
}

void MNVIC_vDisable_Peripheral_INT(u8 A_u8Position)
{
	SET_BIT(NVIC->ICERx[A_u8Position/32] , A_u8Position%32 ) ;

}


/*
 * trigger EXTI0 while the CPU is right in the middle of executing a mathematical calculation inside a Timer interrupt
 * The Cortex-M4 handles this traffic jam using Pending and Active states
 * 1.When an interrupt signal physically arrives at the NVIC
 * but the CPU is busy with a higher-priority task
 * 2.NVIC sets a hardware bit in the ISPR (Interrupt Set-Pending Register)
 * 3.interrupt just sits there, flagging that it needs attention
 * 4.When the CPU finally finishes its current job, it looks at the ISPR, picks the highest priority pending task
 */
void MNVIC_vSetPendingFlag(u8 A_u8Position)
{
	SET_BIT(NVIC->ISPRx[A_u8Position/32] , A_u8Position%32 ) ;

}
void MNVIC_vCLRPendingFlag(u8 A_u8Position)
{
	SET_BIT(NVIC->ICPRx[A_u8Position/32] , A_u8Position%32 ) ;

}
//ACTIVE FLAG
/*
 * The moment the CPU jumps to your C function
 * This tells the rest of the hardware
 * The CPU is currently working on this specific interrupt right now.
 * Normally, the NVIC hardware sets and clears these flags entirely on its own.
 * however, we can manually simulate hardware interrupts using software.
 * By forcing a bit high in the ISPR, we can trick the CPU into thinking a physical sensor just fired
 * good for testing
 */
u8 MNVIC_vGetFlagStatus(u8 A_u8Position)
{
	return GET_BIT(NVIC->IAPRx[A_u8Position/32] , A_u8Position%32 ) ;

}

// SCB
/*
 * Preemption Priority: If Interrupt A has a higher preemption priority than Interrupt B
 * A will forcefully pause B, execute, and then return control to B
 * Sub-Priority:If Interrupt A and Interrupt B have the same preemption priority,
 * if they both arrive at the waiting room (Pending State) at the exact same time,
 * the one with the higher sub-priority gets executed first.
 * ########################################
 * The Cortex-M4 allocates a 4-bit priority field in silicon for the STM32F401
 * You can dedicate all 4 bits to Preemption
 * You can split them into 3 bits Preemption (8 levels) and 1 bit Sub-priority (2 levels), and so on
 * The Security Lock (VECTKEY)
 * ARM put a hardware lock on it. If you try to write to the AIRCR without writing the exact hex value 0x05FA\
 *  to the upper 16 bits in the exact same clock cycle,
 *  the silicon ignores the write entirely
 */


// 1. Configure SCB_AIRCR for 3 Preemption bits and 1 Sub-priority bit
//MNVIC_SetGroupPriority(Group8Sub2);
void MNVIC_SetGroupPriority(NVIC_Group_t A_xGroupPriority)
{
	G_u8GroupPriority = A_xGroupPriority;
	SCB_AIRCR = VECTKEY | (A_xGroupPriority<<8);
}

//2. Set EXTI0 (Position 6) to Preemption Priority 3, Sub-priority 1
//MNVIC_vSetPeripheralPriority(6, 3, 1);

void MNVIC_vSetPeripheralPriority(u8 A_u8Position, u8 A_u8Group, u8 A_u8SubGroup)
{
	switch(G_u8GroupPriority)
	{
	case Group16Sub0:
		NVIC->IPRx[A_u8Position] = A_u8Group <<4;
		break;
	case Group8Sub2:
		NVIC->IPRx[A_u8Position] = A_u8Group <<5 | A_u8SubGroup <<4;
		break;
	case Group4Sub4:
		NVIC->IPRx[A_u8Position] = A_u8Group <<6 | A_u8SubGroup <<4;
		break;
	case Group2Sub8:
		NVIC->IPRx[A_u8Position] = A_u8Group <<7 | A_u8SubGroup <<4;
		break;
	case Group0Sub16:
		NVIC->IPRx[A_u8Position] = A_u8SubGroup <<4;
		break;
	}
}
