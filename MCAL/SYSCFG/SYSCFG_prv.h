#ifndef MCAL_SYSCFG_SYSCFG_PRV_H_
#define MCAL_SYSCFG_SYSCFG_PRV_H_

#define SYSCFG_BASE_ADDR		0x40013800U

typedef struct{
    volatile u32 MEMRMP;
    volatile u32 PMC;
    volatile u32 EXTICRx[4];
    volatile u32 Reserved[2];
    volatile u32 CMPCR;
} SYSCFG_MemMap_t;

#define SYSCFG		((volatile SYSCFG_MemMap_t*)(SYSCFG_BASE_ADDR))




#endif /* MCAL_SYSCFG_SYSCFG_PRV_H_ */
