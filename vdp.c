#define _SPR2_
#include "sega_spr.h"
#include "sega_scl2.h"
#include "shared.h"
//#include "cz80/cz80.h"

//#define ACTION_REPLAY 1 //utile
extern SprSpCmd  smsSprite[64+3];
extern Uint16 *colAddr; 
extern Uint16 *colBgAddr; 
extern Uint8 *aVRAM;
extern unsigned int hz;
#ifndef ACTION_REPLAY
static Uint16 *name_lut;//name_lut[0x10000];
#else
static Uint16 name_lut[0x10000];
#endif
static Uint32 bp_lut[0x10000];
static Uint16 cram_lut[0x40];
static Uint16 *ss_map =(Uint16 *)SCL_VDP2_VRAM_B0;;
extern int scroll_x,scroll_y;
extern t_sms sms;
unsigned char *cache;
#define ALIGN_DWORD
#define	RGB( r, g, b )		(0x8000U|((b) << 10)|((g) << 5 )|(r))
/* VDP context */
t_vdp vdp;

/* Return values from the V counter */
uint8 vcnt[0x200] =
{
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
    0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
    0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
    0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA,
                                  0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
    0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
};

/* Return values from the H counter */
uint8 hcnt[0x200] =
{
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
    0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
    0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
    0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
    0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9,
                      0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
    0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
    0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
    0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
    0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
};
//-------------------------------------------------------------------------------------------------------------------------------------
static __inline__ void	_spr2_transfercommand()
{
	memcpyl(aVRAM,smsSprite,0x860 ) ;
}
/* Reset VDP emulation */
void vdp_reset(void)
{
    memset(&vdp, 0, sizeof(t_vdp));
}
/* Write data to the VDP's control port */
void vdp_ctrl_w(int data)
{
    /* Waiting for the reset of the command? */
    if(vdp.pending == 0)
    {
        /* Save data for later */
        vdp.latch = data;

        /* Set pending flag */
        vdp.pending = 1;
    }
    else
    {
        /* Clear pending flag */
        vdp.pending = 0;

        /* Extract code bits */
        vdp.code = (data >> 6) & 3;

        /* Make address */
        vdp.addr = (data << 8 | vdp.latch) & 0x3FFF;

        /* Read VRAM for code 0 */
        if(vdp.code == 0)
        {
            /* Load buffer with current VRAM byte */
            vdp.buffer = vdp.vram[vdp.addr & 0x3FFF];

            /* Bump address */
            vdp.addr = (vdp.addr + 1) & 0x3FFF;
        }

        /* VDP register write */
//        if((data & 0xF0) == 0x80)
        if(vdp.code == 2)
        {
            int r = (data & 0x0F);
            /* Store register data */
            vdp.reg[r] = vdp.latch;
			switch(r)
			{
				case 8 :
//					x =  ((vdp.reg[r]) ^ 0xff) & 0xff;
					scroll_x =  ((vdp.reg[r]) ^ 0xff) ;
					scroll_x<<=16;
					break;

				case 9 :
					scroll_y = (vdp.reg[r]&0xff)<<16;
					break;
				/* Update table addresses */
				case 2 :
					vdp.ntab = (vdp.reg[2] << 10) & 0x3800;
					break;

				case 5 :
					vdp.satb = (vdp.reg[5] << 7) & 0x3F00;
					break;
			}
        }
    }
}

/* Read the status flags */
int vdp_ctrl_r(void)
{
    /* Save the status flags */
    uint8 temp = vdp.status;

    /* Clear pending flag */
    vdp.pending = 0;
	vdp.status = 0;
	z80_lower_IRQ();
    /* Return the old status flags */
    return (temp);
}


/* Write data to the VDP's data port */
void vdp_data_w(int data)
{
    int index;
    int delta;

    /* Clear the pending flag */
    vdp.pending = 0;

    switch(vdp.code)
    {
        case 0: /* VRAM write */
        case 1: /* VRAM write */
        case 2: /* VRAM write */

            /* Get current address in VRAM */
            index = (vdp.addr & 0x3FFF);

            /* Only update if data is new */
            if(data != vdp.vram[index])
            {
				/* Store VRAM byte */
                vdp.vram[index] = data;

//				if(index>=vdp.ntab && index<vdp.ntab+0x700)
// VBT 04/02/2007 : modif compilo
				if(index>=vdp.ntab)
					if( index<vdp.ntab+0x700)
				{
					int row,column;
					word temp;
					delta =index - vdp.ntab;
					row = delta & 0x7C0;	  //					row = (delta >> 6) & 0x1F;
					column = (delta>>1) & 0x1F;
					temp = *(uint16 *)&vdp.vram[index&~1];
//					delta = (row<<6)+column;
					delta = row+column;
					ss_map[delta] =ss_map[delta+32] =ss_map[0x700+delta] =ss_map[0x700+delta+32] =name_lut[temp];
				}
                /* Mark patterns as dirty */
        uint32 bp = *(uint32 *)&vdp.vram[index & ~3];
        uint32 *pg = (uint32 *) &cache[0x00000 | (index & ~3)];
        uint32 *sg = (uint32 *)&aVRAM[0x880 + (index & ~3)];
        uint32 temp1 = bp_lut[bp & 0xFFFF];
        uint32 temp2 = bp_lut[(bp>>16) & 0xFFFF];
        *sg= *pg = (temp1<<2 | temp2 );
//        *pg = (temp1<<2 | temp2 );

            }

//VBT : A REMETTRE A LA PLACE  de rederSprite des que le probleme sur yp=208 est r�solu
// VBT04/02/2007 modif compilo
			if(index>=vdp.satb )
				if( index < vdp.satb+0x40)
			{
				SprSpCmd *smsSpritePtr;
				smsSpritePtr = &smsSprite[3];

				// Sprite dimensions 
				int height = (vdp.reg[1] & 0x02) ? 16 : 8;

				delta=(index-vdp.satb);
			// Pointer to sprite attribute table 
				byte *st = (byte *)&vdp.vram[vdp.satb];
				// Sprite Y position 
				int yp = st[delta];

				if(yp == 208) 
				{
					
					smsSpritePtr[delta].control = CTRL_END;
					smsSpritePtr[delta].drawMode = 0;
					smsSpritePtr[delta].charAddr	= 0;
					smsSpritePtr[delta].charSize		= 0;
					smsSpritePtr[delta].ax	= 0;
					smsSpritePtr[delta].ay	= 0;
//					nbSprites = delta+5;
//ajouter un flag
					break;
				}
				//Actual Y position is +1 
				yp ++;
				//Wrap Y coordinate for sprites > 240 
				if(yp > 240) yp -= 256;
				smsSpritePtr[delta].ay = yp;

				// Clip sprites on left edge 
				smsSpritePtr[delta].control = ( JUMP_NEXT | FUNC_NORMALSP);
				smsSpritePtr[delta].drawMode   = ( COLOR_0 | ECD_DISABLE | COMPO_REP);		
				smsSpritePtr[delta].charSize   = 0x100+ height;  //0x100
			}
// VBT 04/02/2007 : modif compilo
			if(index>=vdp.satb+0x80)
				if(index < vdp.satb+0x100)
			{
				byte *st = (byte *)&vdp.vram[vdp.satb];
				delta=((index-(vdp.satb+0x80)))>>1;

				if((index-vdp.satb) &1)
				{
					int n = st[0x81 + (delta << 1)];
					//Add MSB of pattern name 
					if(vdp.reg[6] & 0x04) n |= 0x0100;
					//Mask LSB for 8x16 sprites 
					if(vdp.reg[1] & 0x02) n &= 0x01FE;

					smsSprite[delta+3].charAddr   =  0x110+(n<<2);
				}
				else
				{
						//Sprite X position 
					int xp = st[0x80 + (delta << 1)];
					//X position shift 
					if(vdp.reg[0] & 0x08) xp -= 8;
					smsSprite[delta+3].ax = xp;
				}

			}

            break;

        case 3: /* CRAM write */
            index = (vdp.addr & 0x1F);
            if(data != vdp.cram[index])
            {
                vdp.cram[index] = data;
				colBgAddr[index] = cram_lut[data & 0x3F];

				  if (index>0x0f)
					colAddr[index&0x0f] =  colBgAddr[index];
            }
            break;
    }

    /* Bump the VRAM address */
//     vdp.addr ++;
		vdp.addr =(vdp.addr + 1) & 0x3FFF;
}

/* Read data from the VDP's data port */
int vdp_data_r(void)
{
    uint8 temp = 0;
    vdp.pending = 0;
    temp = vdp.buffer;
//    vdp.buffer = vdp.vram[(vdp.addr & 0x3FFF)^1];
    vdp.buffer = vdp.vram[(vdp.addr & 0x3FFF)];
    vdp.addr = (vdp.addr + 1) & 0x3FFF;
    return temp;
}



/* Process frame events */
void vdp_run(void)
//void vdp_run(t_vdp *vdp)
{
    if(vdp.line <= 0xC0)
    {
        if(vdp.line == 0xC0)
        {
            _spr2_transfercommand();
            vdp.status |= 0x80;
        }

		if(vdp.line == 0)
        {
            vdp.left = vdp.reg[10];
        }

        if(vdp.left == 0)
        {
            vdp.left = vdp.reg[10];
            vdp.status |= 0x40;
        }
        else
        {
            vdp.left -= 1;
        }
        if((vdp.status & 0x40) && (vdp.reg[0] & 0x10))
        {
            sms.irq = 1;
//            z80_set_irq_line(0, ASSERT_LINE);
//            Cz80_Set_IRQ(&Cz80_struc, 0);
			z80_raise_IRQ(0);
        }
    }
    else
    {
        vdp.left = vdp.reg[10];

        if((vdp.line < 0xE0) && (vdp.status & 0x80) && (vdp.reg[1] & 0x20))
        {
            sms.irq = 1;
//            z80_set_irq_line(0, ASSERT_LINE);
//              Cz80_Set_IRQ(&Cz80_struc, 0);
            z80_raise_IRQ(0);
        }
    }
}


uint8 vdp_vcounter_r(void)
{
    return (vcnt[(vdp.line & 0x1FF)]);
}


uint8 vdp_hcounter_r(void)
{
//    int pixel = (((z80_ICount % CYCLES_PER_LINE) / 4) * 3) * 2;
//  int pixel = (((Cz80_struc.CycleIO % CYCLES_PER_LINE) / 4) * 3) * 2;
  int pixel;
  if (hz==60)
	pixel = (((z80_get_cycles_elapsed() % CYCLES_PER_LINE_60) >>2) * 3) <<1;
  else
	pixel = (((z80_get_cycles_elapsed() % CYCLES_PER_LINE_50) >>2) * 3) <<1;
//	printf("Cz80_struc.CycleIO %d\n pixel %d\n",Cz80_struc.CycleIO,pixel);
    return (hcnt[((pixel >> 1) & 0x1FF)]);
}


void make_name_lut(long filesize)
{
#ifndef ACTION_REPLAY
//	if (filesize > 0x40000 && filesize <=0x80000)
// VBT 11/11/2011 : � remettre si on a pas assez de m�moire

	if (filesize ==0x80000)
		name_lut = (Uint16 *)0x00200000;
	else
		name_lut = (Uint16 *)0x0607F000; 
#endif

	int i, j;
	for(j = 0; j < 0x10000; j++)
	{
		i = ((j >> 8) & 0xFF) | ((j  & 0xFF) <<8);
		int name = (i & 0x1FF);
		int flip = (i >> 9) & 3;
		int pal = (i >> 11) & 1;
		name_lut[j] = (pal << 12 | flip << 10 | name);
	}
}

void make_bp_lut(void)
{
    int i, j;
    for(j = 0; j < 0x10000; j++)
    {
        uint32 row = 0;
        i = ((j >> 8) & 0xFF) | ((j  & 0xFF) <<8);

        if(i & 0x8000) row |= 0x20000000;
        if(i & 0x4000) row |= 0x02000000;
        if(i & 0x2000) row |= 0x00200000;
        if(i & 0x1000) row |= 0x00020000;
        if(i & 0x0800) row |= 0x00002000;
        if(i & 0x0400) row |= 0x00000200;
        if(i & 0x0200) row |= 0x00000020;
        if(i & 0x0100) row |= 0x00000002;
        if(i & 0x0080) row |= 0x10000000;
        if(i & 0x0040) row |= 0x01000000;
        if(i & 0x0020) row |= 0x00100000;
        if(i & 0x0010) row |= 0x00010000;
        if(i & 0x0008) row |= 0x00001000;
        if(i & 0x0004) row |= 0x00000100;
        if(i & 0x0002) row |= 0x00000010;
        if(i & 0x0001) row |= 0x00000001;
        bp_lut[j] = row;
    }
}

void make_cram_lut(void)
{
    int j;
    for(j = 0; j < 0x40; j++)
    {
        int r = (j >> 0) & 3;
        int g = (j >> 2) & 3;
        int b = (j >> 4) & 3;
        r  = (r << 3) | (r << 1) | (r >> 1);
        g = (g << 3) | (g << 1) | (g >> 1);
        b = (b << 3) | (b << 1) | (b >> 1);
        cram_lut[j] =RGB(r,g,b);
    }
}


