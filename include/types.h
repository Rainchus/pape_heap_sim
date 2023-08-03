#ifndef _TYPES_H
#define _TYPES_H

#include "evt.h"

typedef unsigned char			u8;	/* unsigned  8-bit */
typedef unsigned short			u16;	/* unsigned 16-bit */
typedef unsigned long			u32;	/* unsigned 32-bit */
typedef unsigned long long		u64;	/* unsigned 64-bit */

typedef signed char			s8;	/* signed  8-bit */
typedef short				s16;	/* signed 16-bit */
typedef long				s32;	/* signed 32-bit */
typedef long long			s64;	/* signed 64-bit */

typedef volatile unsigned char		vu8;	/* unsigned  8-bit */
typedef volatile unsigned short		vu16;	/* unsigned 16-bit */
typedef volatile unsigned long		vu32;	/* unsigned 32-bit */
typedef volatile unsigned long long	vu64;	/* unsigned 64-bit */

typedef volatile signed char		vs8;	/* signed  8-bit */
typedef volatile short			vs16;	/* signed 16-bit */
typedef volatile long			vs32;	/* signed 32-bit */
typedef volatile long long		vs64;	/* signed 64-bit */

typedef float				f32;	/* single prec floating point */
typedef double				f64;	/* double prec floating point */

typedef s32 Bytecode;
typedef Bytecode EvtScript[];
typedef s32 b32;
typedef s8 b8;
typedef s32 HitID;
typedef u32 AnimID;

typedef s32 ApiStatus;
typedef ApiStatus(*ApiFunc)(struct Evt*, s32);

#define UNK_PTR void*
#define NULL 0
#define FALSE 0
#define TRUE 1
#define ALIGN16(val) (((val) + 0xF) & ~0xF)

#define MSG_PTR u8*
#define IMG_PTR u8*
#define PAL_PTR u16*

#define MSG_BIN u8
#define IMG_BIN u8
#define PAL_BIN u16

// Alternative to libultra's M_PI: non-float version; more digits cause issues
#define PI      3.141592f
#define PI_D    3.141592
#define TAU     6.28318f
#define PI_S    3.14159f // Shorter PI

#define DEG_TO_RAD(deg) (((deg) * TAU) / 360.0f)

#endif