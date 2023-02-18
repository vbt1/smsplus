#include    <machine.h>
//#include <string.h>

#define		_SPR3_
#include "sega_spr.h"
#include "sega_scl2.h"
#include "shared.h"

//#define ACTION_REPLAY 1			//unused

/* Pattern cache */
// byte *cache;
//Uint16 cram_lut[0x40];
//#ifndef ACTION_REPLAY
//Uint16 *name_lut;//Uint16 name_lut[0x10000];
//#else
//Uint16 name_lut[0x10000];
//#endif
//Uint32 bp_lut[0x10000]; 

// extern Uint16 *colAddr; 
//extern int oversprite;
/* Dirty pattern info */
//byte vram_dirty[0x200];
//byte is_vram_dirty;
//extern Uint8 *aVRAM;

//#define	RGB( r, g, b )		(0x8000U|((b) << 10)|((g) << 5 )|(r))
/* Macros to access memory 32-bits at a time (from MAME's drawgfx.c) */
#define ALIGN_DWORD

/* Reset the rendering data */
/*
void render_reset(void)
{
//    int i;
    // Clear display bitmap 
    // Clear palette 
//    for(i = 0; i < 262; i += 1)
//		ls_tbl[i] =0;
//        palette_sync(i);

    // Invalidate pattern cache 
//    is_vram_dirty = 1;


// VBT faire un memset de aVRAM

//	memset(vram_dirty, 1, 0x200);
	memset(cache, 0, sizeof(cache));
}
*/

//------------------------------------------------------------------------------
/* Update pattern cache with modified tiles */
/*
void update_cache(void)
{
    int i, x, y, c;
    int b0, b1, b2, b3;
    int i0, i1, i2, i3,i4,i5,i6,i7;

    if(!is_vram_dirty) 
        return;

	is_vram_dirty = 0;

    for(i = 0; i < 0x200; i ++)
    {
        if(vram_dirty[i])
        {
            vram_dirty[i] = 0;

            for(y = 0; y < 8; y ++)
            {
				uint16 index = (i << 5) | (y << 2);

//                b0 = vdp.vram[((i << 5) | (y << 2) | (0))];
//                b1 = vdp.vram[((i << 5) | (y << 2) | (1))];
//                b2 = vdp.vram[((i << 5) | (y << 2) | (2))];
//                b3 = vdp.vram[((i << 5) | (y << 2) | (3))];

				b0 = vdp.vram[index | (0)];
                b1 = vdp.vram[index | (1)];
                b2 = vdp.vram[index | (2)];
                b3 = vdp.vram[index | (3)];

                for(x = 0; x < 8; x +=2)
                {
                    i4 = (b0 >> (x ^ 7)) & 1;
                    i5 = (b1 >> (x ^ 7)) & 1;
                    i6 = (b2 >> (x ^ 7)) & 1;
                    i7 = (b3 >> (x ^ 7)) & 1;

					i0 = (b0 >> ((x + 1) ^ 7)) & 1;
					i1 = (b1 >> ((x + 1) ^ 7)) & 1;
					i2 = (b2 >> ((x + 1) ^ 7)) & 1;
					i3 = (b3 >> ((x + 1) ^ 7)) & 1;

					c = (i7 << 7 | i6 << 6 | i5 << 5 | i4 << 4 | i3 << 3 | i2 << 2 | i1 << 1 | i0);

//                  cache[0x00000 | (i << 5) | ((y  ) << 2) | ( x >> 1)] = c;

					cache[0x00000 | (index) | ( x >> 1)] = c;
					aVRAM[0x880 +( (index) | ( x >> 1))] = c;
                }
			}

					cache[0x00000 | (index) | ( x >> 1)] = c;
					aVRAM[0x880 +( (index) | ( x >> 1))] = c;
		}
    }
}
*/

