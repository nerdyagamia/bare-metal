#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"
#include "DMA_int.h"
#include "DMA_prv.h"

static void (*G_Fptr[8])(void)={NULL};

void MDMA2_vInit(u8 A_u8StreamID)
{
    /* Disable stream to allow configuration */
    CLR_BIT(DMA2->Str[A_u8StreamID].CR,0);
    while(GET_BIT(DMA2->Str[A_u8StreamID].CR,0) == 1);
}

void MDMA2_vSetStreamCfg(
    u8 A_u8StreamID,
    u8 A_u8ChannelID,
    u32 *A_u32SourceAddr,
    u32 *A_u32DestAddr,
    Size_t A_xPSize,
    Size_t A_xMSize,
    u16 A_u16BlockSize,
    Threshold_t A_xThreshold,
    Direction_t A_xDirection,
    u8 A_u8PINC,
    u8 A_u8MINC
)
{
    /* 1. Select Channel */
    DMA2->Str[A_u8StreamID].CR &= ~(0b111 << 25);
    DMA2->Str[A_u8StreamID].CR |= (A_u8ChannelID << 25);

    /* 2. Set direction */
    DMA2->Str[A_u8StreamID].CR &= ~(0b11 << 6);
    DMA2->Str[A_u8StreamID].CR |= (A_xDirection << 6);

    /* 3. Set Increment Modes */
    if(A_u8PINC) SET_BIT(DMA2->Str[A_u8StreamID].CR, 9);
    else CLR_BIT(DMA2->Str[A_u8StreamID].CR, 9);

    if(A_u8MINC) SET_BIT(DMA2->Str[A_u8StreamID].CR, 10);
    else CLR_BIT(DMA2->Str[A_u8StreamID].CR, 10);

    /* 4. Set Addresses (If Memory to Periph, Source is Mem, Dest is Periph) */
    if (A_xDirection == MemToPeriph) {
        DMA2->Str[A_u8StreamID].PAR  = (u32)A_u32DestAddr;
        DMA2->Str[A_u8StreamID].M0AR = (u32)A_u32SourceAddr;
    } else {
        DMA2->Str[A_u8StreamID].PAR  = (u32)A_u32SourceAddr;
        DMA2->Str[A_u8StreamID].M0AR = (u32)A_u32DestAddr;
    }

    /* 5. Select sizes */
    DMA2->Str[A_u8StreamID].CR &= ~(0b11 << 11);
    DMA2->Str[A_u8StreamID].CR |= (A_xPSize << 11);
    DMA2->Str[A_u8StreamID].CR &= ~(0b11 << 13);
    DMA2->Str[A_u8StreamID].CR |= (A_xMSize << 13);

    /* 6. Block size */
    DMA2->Str[A_u8StreamID].NDTR = A_u16BlockSize;

    /* 7. Threshold */
    DMA2->Str[A_u8StreamID].FCR &= ~(0b11);
    DMA2->Str[A_u8StreamID].FCR |= A_xThreshold;

    /* 8. Enable Transfer Complete Interrupt */
    SET_BIT(DMA2->Str[A_u8StreamID].CR, 4);
}

void MDMA2_vEnableStream(u8 A_u8StreamID)
{
    /* Clear stream interrupt flags before starting */
    if(A_u8StreamID == 0) {
        DMA2->LIFCR |= (1U<<0) | (1U<<2) | (1U<<3) | (1U<<4) | (1U<<5);
    } else if (A_u8StreamID == 7) {
        DMA2->HIFCR |= (1U<<22) | (1U<<24) | (1U<<25) | (1U<<26) | (1U<<27);
    }
    SET_BIT(DMA2->Str[A_u8StreamID].CR,0);
}

void MDMA2_vCallBack(u8 A_u8StreamID, void(*A_xFptr)(void))
{
    G_Fptr[A_u8StreamID] = A_xFptr;
}

/* Original Stream 0 Handler */
void DMA2_Stream0_IRQHandler(void)
{
    if(G_Fptr[0] != NULL) G_Fptr[0]();
    DMA2->LIFCR |= (1U<<0) | (1U<<2) | (1U<<3) | (1U<<4) | (1U<<5);
}

/* Added Stream 7 Handler for USART1_TX */
void DMA2_Stream7_IRQHandler(void)
{
    if(G_Fptr[7] != NULL) G_Fptr[7]();
    DMA2->HIFCR |= (1U<<22) | (1U<<24) | (1U<<25) | (1U<<26) | (1U<<27);
}
