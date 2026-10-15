#ifndef STD_TYPES_H_
#define STD_TYPES_H_

//we are giving each type a strictly guaranteed number of bits.

// 8 bits
typedef unsigned char u8;
typedef signed char s8;

// 16 bits
typedef unsigned short int u16;
typedef signed short int s16;

// 32 bits
typedef unsigned long int u32;
typedef signed long int s32;

// 64 bits
typedef unsigned long long int u64;

// floating points
typedef float f32;
typedef double f64;

// Standard return types
typedef u8 Std_ReturnType;

#define E_OK       (Std_ReturnType)0
#define E_NOT_OK   (Std_ReturnType)1


#ifndef NULL
#define NULL    ((void*)0)
#endif



#endif /* STD_TYPES_H_ */
