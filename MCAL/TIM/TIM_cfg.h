/**************************************************
* Author: Khaled Ahmed Elwan
* Date: 19/9/2026
* Version: 2.0
* Description: TIM_config.h
***************************************************/
#ifndef TIM_CONFIG_H
#define TIM_CONFIG_H

#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"


/*******< Prescaler (PSC) >**********************************************/
#define TIM2_PSC    83
#define TIM3_PSC    83
#define TIM4_PSC    83
#define TIM5_PSC    83
#define TIM9_PSC    83
#define TIM10_PSC   83
#define TIM11_PSC   83

/*******< Auto reload (ARR) >********************************************
 * TIM2 / TIM5 : up to 0xFFFFFFFF (32-bit)
 * others      : up to 0xFFFF     (16-bit)
 ***************************************************************************/
#define TIM2_ARR    1000
#define TIM3_ARR    1000
#define TIM4_ARR    1000
#define TIM5_ARR    1000
#define TIM9_ARR    1000
#define TIM10_ARR   1000
#define TIM11_ARR   1000

/*******< Direction (TIM2..TIM5 only) >***********************************
 *  0 : up counter
 *  1 : down counter
 ***************************************************************************/
#define TIM2_DIR    0
#define TIM3_DIR    0
#define TIM4_DIR    0
#define TIM5_DIR    0

/*******< Alignment (TIM2..TIM5 only) >***********************************
 *  EDGE_ALIGNED
 *  CENTER_ALIGNED_MODE_1
 *  CENTER_ALIGNED_MODE_2
 *  CENTER_ALIGNED_MODE_3
 ***************************************************************************/
#define TIM2_MODE   EDGE_ALIGNED
#define TIM3_MODE   EDGE_ALIGNED
#define TIM4_MODE   EDGE_ALIGNED
#define TIM5_MODE   EDGE_ALIGNED

/*******< Auto-reload preload (ARPE) >************************************
 *  ENABLE_BUFFER / DISABLE_BUFFER
 ***************************************************************************/
#define TIM2_BUFFER_MODE    DISABLE_BUFFER
#define TIM3_BUFFER_MODE    DISABLE_BUFFER
#define TIM4_BUFFER_MODE    DISABLE_BUFFER
#define TIM5_BUFFER_MODE    DISABLE_BUFFER
#define TIM9_BUFFER_MODE    DISABLE_BUFFER
#define TIM10_BUFFER_MODE   DISABLE_BUFFER
#define TIM11_BUFFER_MODE   DISABLE_BUFFER

#endif
