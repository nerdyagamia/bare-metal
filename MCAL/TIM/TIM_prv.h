/**************************************************
* Author: Khaled Ahmed Elwan
* Date: 19/9/2026
* Version: 2.0
* Description: TIM_private.h  (STM32F401RCT6 - TIM2..TIM5, TIM9..TIM11)
***************************************************/
#ifndef TIM_PRIVATE_H
#define TIM_PRIVATE_H

#include "../../LIB/STD_TYPES.h"

/**************************< Base addresses >******************************/
/* (wrapped with #ifndef so a host-side unit test can override them) */
#ifndef TIM2_BASE_ADDRESS
#define TIM2_BASE_ADDRESS   0x40000000UL    /* APB1 */
#define TIM3_BASE_ADDRESS   0x40000400UL    /* APB1 */
#define TIM4_BASE_ADDRESS   0x40000800UL    /* APB1 */
#define TIM5_BASE_ADDRESS   0x40000C00UL    /* APB1 */
#define TIM9_BASE_ADDRESS   0x40014000UL    /* APB2 */
#define TIM10_BASE_ADDRESS  0x40014400UL    /* APB2 */
#define TIM11_BASE_ADDRESS  0x40014800UL    /* APB2 */
#endif

/**************************< Register map >********************************
 * One struct for all 7 timers. TIM2..TIM5 are the full version, TIM9..TIM11
 * are subsets that keep the SAME offsets, so a single struct works for all.
 * Registers marked "not present" must not be touched on that timer
 * (the capability table in TIM_program.c protects against this).
 **************************************************************************/
typedef struct TIM_RegDef_t
{
    volatile u32 CR1;        /* 0x00                                   */
    volatile u32 CR2;        /* 0x04  not present in TIM9/10/11        */
    volatile u32 SMCR;       /* 0x08  not present in TIM10/11          */
    volatile u32 DIER;       /* 0x0C                                   */
    volatile u32 SR;         /* 0x10                                   */
    volatile u32 EGR;        /* 0x14                                   */
    volatile u32 CCMR1;      /* 0x18                                   */
    volatile u32 CCMR2;      /* 0x1C  not present in TIM9/10/11        */
    volatile u32 CCER;       /* 0x20                                   */
    volatile u32 CNT;        /* 0x24                                   */
    volatile u32 PSC;        /* 0x28                                   */
    volatile u32 ARR;        /* 0x2C                                   */
    volatile u32 RESERVED0;  /* 0x30                                   */
    volatile u32 CCR1;       /* 0x34                                   */
    volatile u32 CCR2;       /* 0x38  not present in TIM10/11          */
    volatile u32 CCR3;       /* 0x3C  not present in TIM9/10/11        */
    volatile u32 CCR4;       /* 0x40  not present in TIM9/10/11        */
    volatile u32 RESERVED1;  /* 0x44                                   */
    volatile u32 DCR;        /* 0x48  not present in TIM9/10/11        */
    volatile u32 DMAR;       /* 0x4C  not present in TIM9/10/11        */
    volatile u32 OR;         /* 0x50  TIM2, TIM5, TIM11 only           */
} TIM_RegDef_t;

#define TIM2    ((TIM_RegDef_t*)TIM2_BASE_ADDRESS)
#define TIM3    ((TIM_RegDef_t*)TIM3_BASE_ADDRESS)
#define TIM4    ((TIM_RegDef_t*)TIM4_BASE_ADDRESS)
#define TIM5    ((TIM_RegDef_t*)TIM5_BASE_ADDRESS)
#define TIM9    ((TIM_RegDef_t*)TIM9_BASE_ADDRESS)
#define TIM10   ((TIM_RegDef_t*)TIM10_BASE_ADDRESS)
#define TIM11   ((TIM_RegDef_t*)TIM11_BASE_ADDRESS)

/**************************< Bit positions >*******************************/
/* CR1 */
#define TIM_CR1_CEN     0
#define TIM_CR1_UDIS    1
#define TIM_CR1_URS     2
#define TIM_CR1_OPM     3
#define TIM_CR1_DIR     4       /* TIM2..TIM5 only */
#define TIM_CR1_CMS0    5       /* TIM2..TIM5 only */
#define TIM_CR1_CMS1    6       /* TIM2..TIM5 only */
#define TIM_CR1_ARPE    7

/* DIER / SR / EGR */
#define TIM_DIER_UIE    0
#define TIM_SR_UIF      0
#define TIM_EGR_UG      0

/* SMCR: Slave mode selection (SMS, bits 2:0). Not present on TIM10/TIM11. */
#define TIM_SMCR_SMS0   0
#define TIM_SMCR_SMS1   1
#define TIM_SMCR_SMS2   2

/* SMS values used for the encoder interface (SMCR.SMS) */
#define TIM_ENCODER_MODE_TI1    0x01   /* count on TI1 edges only        */
#define TIM_ENCODER_MODE_TI2    0x02   /* count on TI2 edges only        */
#define TIM_ENCODER_MODE_TI12   0x03   /* count on both TI1 and TI2 (x4) */

/* CCMR1: CC1S / CC2S (channel direction, 2 bits each) */
#define TIM_CCMR1_CC1S0  0
#define TIM_CCMR1_CC2S0  8

/**************************< Config values >*******************************/
#define EDGE_ALIGNED            0x00
#define CENTER_ALIGNED_MODE_1   0x01
#define CENTER_ALIGNED_MODE_2   0x02
#define CENTER_ALIGNED_MODE_3   0x03

#define ENABLE_BUFFER           1
#define DISABLE_BUFFER          0

/**************************< NVIC (interrupt controller) >*****************
 * IRQn -> NVIC_ISERx bit: ISER register index = IRQn / 32, bit = IRQn % 32
 *****************************************************************************/
#ifndef NVIC_ISER0_ADDRESS
#define NVIC_ISER0_ADDRESS     0xE000E100UL
#endif

/**************************< Timer IRQ numbers (STM32F401) >***************
 * TIM9/TIM10/TIM11 share their vector with TIM1's break/update/trigger
 * lines. This is fine as long as TIM1 itself is not also generating those
 * events; if TIM1 is used later, the shared handler must check both
 * peripherals' status registers.
 *****************************************************************************/
#define TIM2_IRQn                28
#define TIM3_IRQn                29
#define TIM4_IRQn                30
#define TIM5_IRQn                50
#define TIM1_BRK_TIM9_IRQn       24
#define TIM1_UP_TIM10_IRQn       25
#define TIM1_TRG_COM_TIM11_IRQn  26

#endif
