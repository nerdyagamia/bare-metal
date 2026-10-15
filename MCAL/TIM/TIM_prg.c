/*
 * TIM_prg.c
 *
 *  Created on: Oct 14, 2026
 *      Author: Asus - TYF
 */




/********************************************************************
* Author: Khaled Ahmed Elwan
* Date: 19/9/2026
* Version: 2.0
* Description: TIM_program.c  (STM32F401RCT6 - TIM2..TIM5, TIM9..TIM11)
*********************************************************************/

///***************************< LIB ***********************************/
//#include <stddef.h>
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"
/***************************< MCAL **********************************/
#include "TIM_int.h"
#include "TIM_prv.h"
#include "TIM_cfg.h"

/***************************< REGISTER MAP CHECK **********************
 * Build fails if the struct offsets do not match the reference manual */
//_Static_assert(offsetof(TIM_RegDef_t, DIER)  == 0x0C, "DIER offset");
//_Static_assert(offsetof(TIM_RegDef_t, EGR)   == 0x14, "EGR offset");
//_Static_assert(offsetof(TIM_RegDef_t, CCMR1) == 0x18, "CCMR1 offset");
//_Static_assert(offsetof(TIM_RegDef_t, CCMR2) == 0x1C, "CCMR2 offset");
//_Static_assert(offsetof(TIM_RegDef_t, CCER)  == 0x20, "CCER offset");
//_Static_assert(offsetof(TIM_RegDef_t, CNT)   == 0x24, "CNT offset");
//_Static_assert(offsetof(TIM_RegDef_t, PSC)   == 0x28, "PSC offset");
//_Static_assert(offsetof(TIM_RegDef_t, ARR)   == 0x2C, "ARR offset");
//_Static_assert(offsetof(TIM_RegDef_t, CCR1)  == 0x34, "CCR1 offset");
//_Static_assert(offsetof(TIM_RegDef_t, CCR2)  == 0x38, "CCR2 offset");
//_Static_assert(offsetof(TIM_RegDef_t, CCR3)  == 0x3C, "CCR3 offset");
//_Static_assert(offsetof(TIM_RegDef_t, CCR4)  == 0x40, "CCR4 offset");
//_Static_assert(offsetof(TIM_RegDef_t, DCR)   == 0x48, "DCR offset");
//_Static_assert(offsetof(TIM_RegDef_t, OR)    == 0x50, "OR offset");

/***************************< PRIVATE DATA ***************************/
#define TIM_NUMBER          7
#define TIM_INVALID_INDEX   0xFF

/* Capabilities of each timer (same order as TIM_Id_t) */
typedef struct
{
    u8 channels;    /* number of capture/compare channels        */
    u8 is32Bit;     /* 1 = 32-bit counter, 0 = 16-bit counter    */
    u8 hasDirCms;   /* 1 = CR1 has DIR and CMS bits              */
} TIM_Cap_t;

static const TIM_Cap_t TIM_Cap[TIM_NUMBER] =
{
    /* TIM2  */ { 4, 1, 1 },
    /* TIM3  */ { 4, 0, 1 },
    /* TIM4  */ { 4, 0, 1 },
    /* TIM5  */ { 4, 1, 1 },
    /* TIM9  */ { 2, 0, 0 },
    /* TIM10 */ { 1, 0, 0 },
    /* TIM11 */ { 1, 0, 0 },
};

static TIM_RegDef_t * const TIM_Reg[TIM_NUMBER] =
{
    TIM2, TIM3, TIM4, TIM5, TIM9, TIM10, TIM11
};


static u32 TIM_u32ClkHz = 0;
static u32 TIM_u32GetTimerClock(void)
{
    if (TIM_u32ClkHz == 0)
    {
        u32 Local_u32Freq = 0;

        if (RCC_GetFrequency(&Local_u32Freq) == E_OK)
        {
            TIM_u32ClkHz = Local_u32Freq;
        }
        else
        {
            TIM_u32ClkHz = 84000000UL;   /* Default value */
        }
    }
    return TIM_u32ClkHz;
}

static const u32 TIM_PscCfg[TIM_NUMBER] =
{
    TIM2_PSC, TIM3_PSC, TIM4_PSC, TIM5_PSC, TIM9_PSC, TIM10_PSC, TIM11_PSC
};

static const u32 TIM_ArrCfg[TIM_NUMBER] =
{
    TIM2_ARR, TIM3_ARR, TIM4_ARR, TIM5_ARR, TIM9_ARR, TIM10_ARR, TIM11_ARR
};

static const u8 TIM_BufCfg[TIM_NUMBER] =
{
    TIM2_BUFFER_MODE, TIM3_BUFFER_MODE, TIM4_BUFFER_MODE, TIM5_BUFFER_MODE,
    TIM9_BUFFER_MODE, TIM10_BUFFER_MODE, TIM11_BUFFER_MODE
};

/* DIR and alignment exist only in TIM2..TIM5 (the other entries are unused) */
static const u8 TIM_DirCfg[TIM_NUMBER] =
{
    TIM2_DIR, TIM3_DIR, TIM4_DIR, TIM5_DIR, 0, 0, 0
};

static const u8 TIM_ModeCfg[TIM_NUMBER] =
{
    TIM2_MODE, TIM3_MODE, TIM4_MODE, TIM5_MODE,
    EDGE_ALIGNED, EDGE_ALIGNED, EDGE_ALIGNED
};

/***************************< PRIVATE HELPERS ************************/

/* Timer ID -> array index. Returns TIM_INVALID_INDEX for an invalid ID. */
static u8 TIM_u8GetIndex(TIM_Id_t Copy_Id)
{
    switch (Copy_Id)
    {
        case TIM_2:  return 0;
        case TIM_3:  return 1;
        case TIM_4:  return 2;
        case TIM_5:  return 3;
        case TIM_9:  return 4;
        case TIM_10: return 5;
        case TIM_11: return 6;
        default:     return TIM_INVALID_INDEX;
    }
}

/* CHANNEL_x -> channel index (0..3). Returns TIM_INVALID_INDEX if the channel
 * is invalid or does not exist on this timer (e.g. CHANNEL_3 on TIM10). */
static u8 TIM_u8GetChannelIndex(u8 Copy_Idx, u8 Copy_Channel)
{
    u8 ch;

    switch (Copy_Channel)
    {
        case CHANNEL_1: ch = 0; break;
        case CHANNEL_2: ch = 1; break;
        case CHANNEL_3: ch = 2; break;
        case CHANNEL_4: ch = 3; break;
        default:        return TIM_INVALID_INDEX;
    }

    if (ch >= TIM_Cap[Copy_Idx].channels)
    {
        return TIM_INVALID_INDEX;
    }
    return ch;
}

/* Applies the counter alignment (CR1.CMS bits) */
static void TIM_voidApplyAlignment(TIM_RegDef_t *Copy_Tim, u8 Copy_Mode)
{
    if (Copy_Mode == EDGE_ALIGNED)
    {
        CLR_BIT(Copy_Tim->CR1, TIM_CR1_CMS0);
        CLR_BIT(Copy_Tim->CR1, TIM_CR1_CMS1);
    }
    else if (Copy_Mode == CENTER_ALIGNED_MODE_1)
    {
        SET_BIT(Copy_Tim->CR1, TIM_CR1_CMS0);
        CLR_BIT(Copy_Tim->CR1, TIM_CR1_CMS1);
    }
    else if (Copy_Mode == CENTER_ALIGNED_MODE_2)
    {
        CLR_BIT(Copy_Tim->CR1, TIM_CR1_CMS0);
        SET_BIT(Copy_Tim->CR1, TIM_CR1_CMS1);
    }
    else if (Copy_Mode == CENTER_ALIGNED_MODE_3)
    {
        SET_BIT(Copy_Tim->CR1, TIM_CR1_CMS0);
        SET_BIT(Copy_Tim->CR1, TIM_CR1_CMS1);
    }
}

/*===============================< TIM_voidInit >==========================
 * Applies the static configuration from TIM_config.h.
 * The timer clock must be enabled from the RCC driver before calling this. */
void TIM_voidInit(TIM_Id_t Copy_Id)
{
    u8 idx = TIM_u8GetIndex(Copy_Id);
    if (idx == TIM_INVALID_INDEX) return;

    TIM_RegDef_t *t = TIM_Reg[idx];

    /* 1) Alignment and direction (only TIM2..TIM5 have these bits) */
    if (TIM_Cap[idx].hasDirCms)
    {
        TIM_voidApplyAlignment(t, TIM_ModeCfg[idx]);

        /* DIR is read-only in center-aligned mode */
        if (TIM_ModeCfg[idx] == EDGE_ALIGNED)
        {
            if (TIM_DirCfg[idx] == 0) { CLR_BIT(t->CR1, TIM_CR1_DIR); }
            else                      { SET_BIT(t->CR1, TIM_CR1_DIR); }
        }
    }

    /* 2) Auto-reload preload (ARPE) */
    if (TIM_BufCfg[idx] == ENABLE_BUFFER) { SET_BIT(t->CR1, TIM_CR1_ARPE); }
    else                                  { CLR_BIT(t->CR1, TIM_CR1_ARPE); }

    /* 3) Prescaler and auto-reload values */
    t->PSC = TIM_PscCfg[idx];
    t->ARR = TIM_ArrCfg[idx];
}

/*===============================< TIM_voidSetPrescaler >==================*/
void TIM_voidSetPrescaler(TIM_Id_t Copy_Id, u32 Copy_Psc)
{
    u8 idx = TIM_u8GetIndex(Copy_Id);
    if (idx == TIM_INVALID_INDEX) return;

    TIM_Reg[idx]->PSC = Copy_Psc;
}

/*===============================< TIM_voidSetPeriod >=====================*/
void TIM_voidSetPeriod(TIM_Id_t Copy_Id, u32 Copy_Arr)
{
    u8 idx = TIM_u8GetIndex(Copy_Id);
    if (idx == TIM_INVALID_INDEX) return;

    TIM_Reg[idx]->ARR = Copy_Arr;   /* 16-bit timers ignore the upper bits */
}

/*===============================< TIM_voidStart >=========================*/
void TIM_voidStart(TIM_Id_t Copy_Id)
{
    u8 idx = TIM_u8GetIndex(Copy_Id);
    if (idx == TIM_INVALID_INDEX) return;

    TIM_RegDef_t *t = TIM_Reg[idx];

    SET_BIT(t->EGR, TIM_EGR_UG);    /* Force an update event: loads PSC/ARR/CCR into shadow registers */
    CLR_BIT(t->SR,  TIM_SR_UIF);    /* UG sets UIF, clear it to avoid a spurious interrupt */
    SET_BIT(t->CR1, TIM_CR1_CEN);   /* Start counting */
}

/*===============================< TIM_voidStop >==========================*/
void TIM_voidStop(TIM_Id_t Copy_Id)
{
    u8 idx = TIM_u8GetIndex(Copy_Id);
    if (idx == TIM_INVALID_INDEX) return;

    CLR_BIT(TIM_Reg[idx]->CR1, TIM_CR1_CEN);
}

/*===============================< TIM_voidSetCaptureCompareValue >========
 * Writes the CCR value of the selected channel (duty cycle in PWM mode) */
void TIM_voidSetCaptureCompareValue(TIM_Id_t Copy_Id, u8 Copy_Channel, u32 Copy_CCR)
{
    u8 idx = TIM_u8GetIndex(Copy_Id);
    if (idx == TIM_INVALID_INDEX) return;

    u8 ch = TIM_u8GetChannelIndex(idx, Copy_Channel);
    if (ch == TIM_INVALID_INDEX) return;

    switch (ch)
    {
        case 0: TIM_Reg[idx]->CCR1 = Copy_CCR; break;
        case 1: TIM_Reg[idx]->CCR2 = Copy_CCR; break;
        case 2: TIM_Reg[idx]->CCR3 = Copy_CCR; break;
        case 3: TIM_Reg[idx]->CCR4 = Copy_CCR; break;
        default: break;
    }
}

/*===================< TIM_voidSetCaptureCompareMode >=====================
 * Selects the channel direction (CCxS, 2 bits):
 *   00 = Output
 *   01 = Input, mapped on TI1
 *   10 = Input, mapped on TI2
 *   11 = Input, mapped on TRC
 * For PWM use 0 (output). Call this before TIM_voidSetOutputCompareMode. */
void TIM_voidSetCaptureCompareMode(TIM_Id_t Copy_Id, u8 Copy_Channel, u8 Copy_Mode)
{
    u8 idx = TIM_u8GetIndex(Copy_Id);
    if (idx == TIM_INVALID_INDEX) return;

    u8 ch = TIM_u8GetChannelIndex(idx, Copy_Channel);
    if (ch == TIM_INVALID_INDEX) return;

    /* CH1/CH2 live in CCMR1, CH3/CH4 live in CCMR2. Odd channels are shifted by 8 bits. */
    volatile u32 *ccmr  = (ch < 2) ? &TIM_Reg[idx]->CCMR1 : &TIM_Reg[idx]->CCMR2;
    u8            shift = (u8)((ch % 2) * 8);

    *ccmr &= ~((u32)0x3 << shift);                           /* Clear the old CCxS value */
    *ccmr |=  ((u32)(Copy_Mode & 0x3) << shift);             /* Write the new one        */
}

/*===================< TIM_voidSetOutputCompareMode >======================
 * Sets OCxM (3 bits) and enables the CCR preload (OCxPE).
 *   PWM Mode 1 = 6 (110): output HIGH while CNT < CCR
 *   PWM Mode 2 = 7 (111): inverted */
void TIM_voidSetOutputCompareMode(TIM_Id_t Copy_Id, u8 Copy_Channel, u8 Copy_Mode)
{
    u8 idx = TIM_u8GetIndex(Copy_Id);
    if (idx == TIM_INVALID_INDEX) return;

    u8 ch = TIM_u8GetChannelIndex(idx, Copy_Channel);
    if (ch == TIM_INVALID_INDEX) return;

    volatile u32 *ccmr  = (ch < 2) ? &TIM_Reg[idx]->CCMR1 : &TIM_Reg[idx]->CCMR2;
    u8            shift = (u8)((ch % 2) * 8);

    *ccmr &= ~((u32)0x7 << (shift + 4));                     /* Clear the old OCxM value */
    *ccmr |=  ((u32)(Copy_Mode & 0x7) << (shift + 4));       /* Write the new OCxM value */
    *ccmr |=  ((u32)1 << (shift + 3));                       /* OCxPE = 1 (CCR preload)  */
}

/*===================< TIM_voidEnableChannel >=============================
 * Sets CCxE: routes the channel output to the pin.
 * The GPIO pin must be configured as Alternate Function. */
void TIM_voidEnableChannel(TIM_Id_t Copy_Id, u8 Copy_Channel)
{
    u8 idx = TIM_u8GetIndex(Copy_Id);
    if (idx == TIM_INVALID_INDEX) return;

    u8 ch = TIM_u8GetChannelIndex(idx, Copy_Channel);
    if (ch == TIM_INVALID_INDEX) return;

    u8 bit = (u8)(ch * 4);                                   /* CCxE */
    SET_BIT(TIM_Reg[idx]->CCER, bit);
}

/*===================< TIM_voidDisableChannel >============================*/
void TIM_voidDisableChannel(TIM_Id_t Copy_Id, u8 Copy_Channel)
{
    u8 idx = TIM_u8GetIndex(Copy_Id);
    if (idx == TIM_INVALID_INDEX) return;

    u8 ch = TIM_u8GetChannelIndex(idx, Copy_Channel);
    if (ch == TIM_INVALID_INDEX) return;

    u8 bit = (u8)(ch * 4);                                   /* CCxE */
    CLR_BIT(TIM_Reg[idx]->CCER, bit);
}

/*===================< TIM_voidSetPolarity >===============================
 * Copy_Polarity: 0 = active high, 1 = active low (CCxP) */
void TIM_voidSetPolarity(TIM_Id_t Copy_Id, u8 Copy_Channel, u8 Copy_Polarity)
{
    u8 idx = TIM_u8GetIndex(Copy_Id);
    if (idx == TIM_INVALID_INDEX) return;

    u8 ch = TIM_u8GetChannelIndex(idx, Copy_Channel);
    if (ch == TIM_INVALID_INDEX) return;

    u8 bit = (u8)(ch * 4 + 1);                               /* CCxP */
    if (Copy_Polarity == 0) { CLR_BIT(TIM_Reg[idx]->CCER, bit); }
    else                    { SET_BIT(TIM_Reg[idx]->CCER, bit); }
}

/*===================< TIM_voidSetPwmFrequency >===========================
 * Calculates PSC and ARR from the requested frequency (in Hz) and applies them.
 *   ticks per period = (PSC + 1) x (ARR + 1)
 * Takes into account the counter width (16/32-bit) and center-aligned mode
 * (in center-aligned mode the counter counts up and down, so the period is
 * 2 x ARR ticks). */
void TIM_voidSetPwmFrequency(TIM_Id_t Copy_Id, u32 Copy_Hz)
{
    u8 idx = TIM_u8GetIndex(Copy_Id);
    if (idx == TIM_INVALID_INDEX || Copy_Hz == 0) return;

    TIM_RegDef_t *t = TIM_Reg[idx];

    u32 ticks = TIM_u32GetTimerClock()/ Copy_Hz;              /* Timer ticks per PWM period */

    if (TIM_Cap[idx].hasDirCms && TIM_ModeCfg[idx] != EDGE_ALIGNED)
    {
        ticks /= 2;                                     /* Center-aligned: up + down = 2 x ARR */
    }
    if (ticks < 2) return;                              /* Requested frequency is too high */

    /* Smallest PSC that lets ARR fit in the counter width */
    u32 psc = TIM_Cap[idx].is32Bit ? 0 : ((ticks - 1) / 65536);
    u32 arr = (ticks / (psc + 1)) - 1;

    t->PSC = psc;
    t->ARR = arr;
    SET_BIT(t->EGR, TIM_EGR_UG);                        /* Apply the new values immediately */
}

/*===================< TIM_voidSetDuty >===================================
 * Sets the duty cycle in percent (0..100), based on the current ARR value.
 * ccr = (ARR + 1) * percent / 100, split into two parts to avoid 32-bit overflow.
 * Call this AFTER TIM_voidSetPwmFrequency (it depends on the current ARR). */
void TIM_voidSetDuty(TIM_Id_t Copy_Id, u8 Copy_Channel, u8 Copy_Percent)
{
    u8 idx = TIM_u8GetIndex(Copy_Id);
    if (idx == TIM_INVALID_INDEX) return;

    if (Copy_Percent > 100) Copy_Percent = 100;

    u32 period = TIM_Reg[idx]->ARR + 1;
    u32 ccr    = (period / 100) * Copy_Percent + ((period % 100) * Copy_Percent) / 100;

    /* 16-bit CCR cannot hold 65536 (100% with ARR = 65535) */
    if (!TIM_Cap[idx].is32Bit && ccr > 0xFFFF)
    {
        ccr = 0xFFFF;
    }

    TIM_voidSetCaptureCompareValue(Copy_Id, Copy_Channel, ccr);
}

/*===================< TIM_voidEncoderInit >================================
 * Configures CH1 (TI1) and CH2 (TI2) as encoder inputs and puts the timer
 * in encoder mode 3 (SMS = 011): counts on every edge of both TI1 and TI2
 * (x4 resolution). CH3/CH4 are left untouched and stay free for other use.
 * Requires a timer with at least 2 channels; rejected on TIM10/TIM11.
 * PSC is not used in encoder mode (CNT is driven by the input edges, not
 * by the internal clock), so it is simply cleared. ARR is set to the
 * counter's maximum value so CNT has the widest possible range before
 * it wraps. Call TIM_voidStart() afterwards to enable the counter. */
void TIM_voidEncoderInit(TIM_Id_t Copy_Id)
{
    u8 idx = TIM_u8GetIndex(Copy_Id);
    if (idx == TIM_INVALID_INDEX) return;
    if (TIM_Cap[idx].channels < 2) return;   /* needs CH1 and CH2 */

    TIM_RegDef_t *t = TIM_Reg[idx];

    /* CH1/CH2 as input, directly mapped on their own TI (CCxS = 01) */
    t->CCMR1 &= ~((u32)0x3 << TIM_CCMR1_CC1S0);
    t->CCMR1 &= ~((u32)0x3 << TIM_CCMR1_CC2S0);
    t->CCMR1 |=  ((u32)0x1 << TIM_CCMR1_CC1S0);
    t->CCMR1 |=  ((u32)0x1 << TIM_CCMR1_CC2S0);

    /* Non-inverted, rising edge on both inputs (CC1P/CC1NP/CC2P/CC2NP = 0).
     * Use TIM_voidSetPolarity(id, CHANNEL_1, 1) later to reverse direction. */
    t->CCER &= ~(((u32)1 << 1) | ((u32)1 << 3) | ((u32)1 << 5) | ((u32)1 << 7));

    /* Enable both capture channels (CC1E, CC2E) */
    t->CCER |= ((u32)1 << 0) | ((u32)1 << 4);

    /* Encoder mode 3: count on both TI1 and TI2 edges */
    t->SMCR &= ~((u32)0x7 << TIM_SMCR_SMS0);
    t->SMCR |=  ((u32)TIM_ENCODER_MODE_TI12 << TIM_SMCR_SMS0);

    t->PSC = 0;                                          /* unused in encoder mode */
    t->ARR = TIM_Cap[idx].is32Bit ? 0xFFFFFFFFu : 0xFFFFu; /* full counter range   */

    SET_BIT(t->EGR, TIM_EGR_UG);
}

/*===================< TIM_voidEncoderReset >===============================
 * Resets the position counter (CNT) back to 0. */
void TIM_voidEncoderReset(TIM_Id_t Copy_Id)
{
    u8 idx = TIM_u8GetIndex(Copy_Id);
    if (idx == TIM_INVALID_INDEX) return;

    TIM_Reg[idx]->CNT = 0;
}

/*===================< TIM_u32EncoderGetCount >=============================
 * Returns the raw position counter. It wraps around (modulo ARR+1) both
 * increasing and decreasing, so to compute speed take the difference
 * between two readings taken a known time apart. */
u32 TIM_u32EncoderGetCount(TIM_Id_t Copy_Id)
{
    u8 idx = TIM_u8GetIndex(Copy_Id);
    if (idx == TIM_INVALID_INDEX) return 0;

    return TIM_Reg[idx]->CNT;
}

/***************************< PERIODIC INTERRUPT (TIME BASE) **************/

/* IRQ number of each timer, same order as TIM_Id_t / TIM_Reg */
static const u8 TIM_IRQn[TIM_NUMBER] =
{
    TIM2_IRQn, TIM3_IRQn, TIM4_IRQn, TIM5_IRQn,
    TIM1_BRK_TIM9_IRQn, TIM1_UP_TIM10_IRQn, TIM1_TRG_COM_TIM11_IRQn
};

/* One user callback slot per timer. NULL = no callback registered. */
static TIM_Callback_t TIM_Callback[TIM_NUMBER] =
{
    NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

/* Enables one IRQ line in the NVIC (IRQn -> ISERx bit) */
static void TIM_voidNvicEnableIRQ(u8 Copy_IRQn)
{
    volatile u32 *iser = (volatile u32 *)(NVIC_ISER0_ADDRESS + 4u * (Copy_IRQn / 32u));
    *iser = ((u32)1 << (Copy_IRQn % 32u));
}

/* Common handler body shared by every timer's IRQHandler: clears the update
 * flag first (so the interrupt does not re-fire immediately), then calls
 * the registered callback if any. */
static void TIM_voidHandleUpdateIRQ(u8 Copy_Idx)
{
    TIM_RegDef_t *t = TIM_Reg[Copy_Idx];

    if (t->SR & (1u << TIM_SR_UIF))
    {
        CLR_BIT(t->SR, TIM_SR_UIF);

        if (TIM_Callback[Copy_Idx] != NULL)
        {
            TIM_Callback[Copy_Idx]();
        }
    }
}

/*===================< TIM_voidSetTimeBaseFrequency >=======================
 * Sets PSC/ARR so the timer's update event happens at Copy_Hz per second.
 * Identical math to TIM_voidSetPwmFrequency (it only touches PSC/ARR/EGR,
 * no channel registers), given a clearer name for non-PWM (interrupt-only)
 * use. Safe to call whether or not any channel is configured. */
void TIM_voidSetTimeBaseFrequency(TIM_Id_t Copy_Id, u32 Copy_Hz)
{
    TIM_voidSetPwmFrequency(Copy_Id, Copy_Hz);
}

/*===================< TIM_voidSetCallback >=================================
 * Registers the function to run inside the timer's update interrupt.
 * Pass NULL to unregister. Keep the callback short: it runs inside an ISR. */
void TIM_voidSetCallback(TIM_Id_t Copy_Id, TIM_Callback_t Copy_Callback)
{
    u8 idx = TIM_u8GetIndex(Copy_Id);
    if (idx == TIM_INVALID_INDEX) return;

    TIM_Callback[idx] = Copy_Callback;
}

/*===================< TIM_voidEnableInterrupt >=============================
 * Enables the update interrupt (DIER.UIE) and the matching NVIC line.
 * Clears any stale pending flag first so enabling it does not immediately
 * fire on an old event (e.g. from a previous TIM_voidSetTimeBaseFrequency
 * call, which forces an update via EGR.UG). */
void TIM_voidEnableInterrupt(TIM_Id_t Copy_Id)
{
    u8 idx = TIM_u8GetIndex(Copy_Id);
    if (idx == TIM_INVALID_INDEX) return;

    TIM_RegDef_t *t = TIM_Reg[idx];

    CLR_BIT(t->SR, TIM_SR_UIF);
    SET_BIT(t->DIER, TIM_DIER_UIE);
    TIM_voidNvicEnableIRQ(TIM_IRQn[idx]);
}

/*===================< TIM_voidDisableInterrupt >============================
 * Stops the callback from firing. The NVIC line is left enabled (harmless,
 * since DIER.UIE = 0 means the peripheral will not request the interrupt). */
void TIM_voidDisableInterrupt(TIM_Id_t Copy_Id)
{
    u8 idx = TIM_u8GetIndex(Copy_Id);
    if (idx == TIM_INVALID_INDEX) return;

    CLR_BIT(TIM_Reg[idx]->DIER, TIM_DIER_UIE);
}

/***************************< IRQ HANDLERS **********************************
 * Names must match the vector table in your startup file exactly.
 * TIM9/10/11 share their vector with TIM1's break/update/trigger lines
 * (see the IRQn comment in TIM_private.h). */
void TIM2_IRQHandler(void)               { TIM_voidHandleUpdateIRQ(0); }
void TIM3_IRQHandler(void)               { TIM_voidHandleUpdateIRQ(1); }
void TIM4_IRQHandler(void)               { TIM_voidHandleUpdateIRQ(2); }
void TIM5_IRQHandler(void)               { TIM_voidHandleUpdateIRQ(3); }
void TIM1_BRK_TIM9_IRQHandler(void)      { TIM_voidHandleUpdateIRQ(4); }
void TIM1_UP_TIM10_IRQHandler(void)      { TIM_voidHandleUpdateIRQ(5); }
void TIM1_TRG_COM_TIM11_IRQHandler(void) { TIM_voidHandleUpdateIRQ(6); }
