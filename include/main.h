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

#define NULL 0
#define FALSE 0
#define TRUE 1
#define ALIGN16(val) (((val) + 0xF) & ~0xF)

typedef struct HeapNode {
    /* 0x00 */ struct HeapNode* next;
    /* 0x04 */ u32 length;
    /* 0x08 */ u16 allocated;
    /* 0x0A */ u16 entryID;
    /* 0x0C */ u32 capacity;
} HeapNode; // size = 0x10