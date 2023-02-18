//#define SLAVE 1
//#undef BIGGEST_ALIGNMENT
//#define BIGGEST_ALIGNMENT 32

#define MAP 1

#ifndef _SHARED_H_
#define _SHARED_H_

typedef unsigned long int dword;
typedef unsigned short int word;
typedef unsigned char byte;

//#include "saturn/syscall.h"    /* for NULL, malloc(), etc. */
//#include "cz80/cz80.h"
#include "sms.h"
#include "vdp.h"
#include "render.h"
#include "system.h"
//#include "sega_sys.h"

#define SAMPLE 7680L

//#define CLEAR_LINE		0		/* clear (a fired, held or pulsed) line */
//#define ASSERT_LINE     1       /* assert an interrupt immediately */
//extern unsigned char *cpu_readmap[8];
//extern unsigned char *cpu_writemap[8];

typedef unsigned char uint8;
typedef unsigned short int uint16;
typedef unsigned long int uint32;

typedef signed char int8;
typedef signed short int int16;
typedef signed long int int32;

byte update_input(void);

#endif /* _SHARED_H_ */
