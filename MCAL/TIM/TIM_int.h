/**************************************************
* Author: Khaled Ahmed Elwan
* Date: 19/9/2026
* Version: 2.0
* Description: TIM_interface.h
***************************************************/
#ifndef TIM_INTERFACE_H
#define TIM_INTERFACE_H

#include "../../LIB/STD_TYPES.h"

/*************************< Timer IDs >***********************************
 * TIM2..TIM5  : APB1 bus, alternate function AF1 (TIM2) / AF2 (TIM3..5)
 * TIM9..TIM11 : APB2 bus, alternate function AF3
 * The timer clock must be enabled from the RCC driver before TIM_voidInit.
 **************************************************************************/
typedef enum {
    TIM_2,
    TIM_3,
    TIM_4,
    TIM_5,
    TIM_9,
    TIM_10,
    TIM_11
} TIM_Id_t;

/*************************< Channels >************************************
 * TIM2..TIM5 : CHANNEL_1..CHANNEL_4
 * TIM9       : CHANNEL_1..CHANNEL_2
 * TIM10/11   : CHANNEL_1 only
 **************************************************************************/
#define CHANNEL_1   1
#define CHANNEL_2   2
#define CHANNEL_3   3
#define CHANNEL_4   4

/*************************< Output compare modes (OCxM) >*****************/
#define TIM_OC_FROZEN              0
#define TIM_OC_ACTIVE_ON_MATCH     1
#define TIM_OC_INACTIVE_ON_MATCH   2
#define TIM_OC_TOGGLE              3
#define TIM_OC_FORCE_INACTIVE      4
#define TIM_OC_FORCE_ACTIVE        5
#define TIM_OC_PWM_MODE1           6
#define TIM_OC_PWM_MODE2           7

/*************************< Base timer >**********************************/
void TIM_voidInit(TIM_Id_t Copy_Id);
void TIM_voidSetPrescaler(TIM_Id_t Copy_Id, u32 Copy_Psc);
void TIM_voidSetPeriod(TIM_Id_t Copy_Id, u32 Copy_Arr);
void TIM_voidStart(TIM_Id_t Copy_Id);
void TIM_voidStop(TIM_Id_t Copy_Id);

/*************************< Capture / compare channels >******************/
void TIM_voidSetCaptureCompareValue(TIM_Id_t Copy_Id, u8 Copy_Channel, u32 Copy_CCR);
void TIM_voidSetCaptureCompareMode(TIM_Id_t Copy_Id, u8 Copy_Channel, u8 Copy_Mode);
void TIM_voidSetOutputCompareMode(TIM_Id_t Copy_Id, u8 Copy_Channel, u8 Copy_Mode);

void TIM_voidEnableChannel(TIM_Id_t Copy_Id, u8 Copy_Channel);
void TIM_voidDisableChannel(TIM_Id_t Copy_Id, u8 Copy_Channel);
void TIM_voidSetPolarity(TIM_Id_t Copy_Id, u8 Copy_Channel, u8 Copy_Polarity);

/*************************< PWM helpers >*********************************/
void TIM_voidSetPwmFrequency(TIM_Id_t Copy_Id, u32 Copy_Hz);
void TIM_voidSetDuty(TIM_Id_t Copy_Id, u8 Copy_Channel, u8 Copy_Percent);

/*************************< Encoder interface >****************************
 * Uses CH1 (TI1) and CH2 (TI2) together, mode TI1&TI2 (x4 resolution).
 * Requires a timer with at least 2 channels (TIM2..TIM5 or TIM9).
 * TIM10/TIM11 (1 channel) are rejected.
 * CH3/CH4 (on timers that have them) stay completely free for PWM
 * or Input Capture and are not touched by these functions.
 *
 * Usage:
 *   TIM_voidEncoderInit(TIM_2);     // sets up CH1/CH2 as encoder inputs
 *   TIM_voidStart(TIM_2);           // starts the counter
 *   ...
 *   u32 pos = TIM_u32EncoderGetCount(TIM_2);
 *
 * To reverse the counting direction without swapping wires, use the
 * existing TIM_voidSetPolarity(id, CHANNEL_1, 1) (inverts CC1P).
 ****************************************************************************/
void TIM_voidEncoderInit(TIM_Id_t Copy_Id);
void TIM_voidEncoderReset(TIM_Id_t Copy_Id);
u32  TIM_u32EncoderGetCount(TIM_Id_t Copy_Id);

/*************************< Periodic interrupt (time base) >***************
 * Lets a timer fire a fixed-period interrupt and run a user callback in it,
 * independent of any PWM/encoder use on the same or other timers.
 *
 * Which timer runs in the interrupt = whichever TIM_Id_t you pass here.
 * Which function executes inside the interrupt = the callback you register
 *   with TIM_voidSetCallback.
 * When the interrupt happens = every time the counter completes one period
 *   (the update event), i.e. every (1 / Copy_Hz) seconds, once
 *   TIM_voidEnableInterrupt + TIM_voidStart have both been called.
 *
 * The driver clears the update flag (SR.UIF) automatically right before
 * calling the callback, so the callback itself does not need to (and must
 * not need to, to avoid re-triggering immediately).
 *
 * Recommended order:
 *   TIM_voidInit(id);
 *   TIM_voidSetTimeBaseFrequency(id, hz);   // sets PSC/ARR only, no channels
 *   TIM_voidSetCallback(id, MyFunction);
 *   TIM_voidEnableInterrupt(id);            // DIER.UIE + NVIC, clears any
 *                                            // stale pending flag first
 *   TIM_voidStart(id);                      // UG + clear UIF + CEN
 *
 * MyFunction must be void MyFunction(void) and should be kept short, since
 * it runs inside an ISR.
 ****************************************************************************/
typedef void (*TIM_Callback_t)(void);

void TIM_voidSetTimeBaseFrequency(TIM_Id_t Copy_Id, u32 Copy_Hz);
void TIM_voidSetCallback(TIM_Id_t Copy_Id, TIM_Callback_t Copy_Callback);
void TIM_voidEnableInterrupt(TIM_Id_t Copy_Id);
void TIM_voidDisableInterrupt(TIM_Id_t Copy_Id);

#endif
