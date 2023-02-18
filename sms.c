#define		_SPR3_
//#define PROFILING 1
#include "shared.h"
#include "sega_scl2.h"
//#include "sega_DMA.h"    
#undef  SCL_MAXLINE
#define	SCL_MAXLINE	 256
//#define DEBUG 1
unsigned dummy_write[0x100];
Fixed32	ls_tbl[3 * SCL_MAXLINE];
//extern int vb;

/* SMS context */
t_sms sms;
//cz80_struc Cz80_struc;
int scroll_x=0,scroll_y=0;
unsigned int first = 1;

#if PROFILING
#include "sega_tim.h"
long bcount2[21],bcount,bcountdiff,bcNum,line;
#endif


/* Run the virtual console emulation for one frame */
void sms_frame(int skip_render)
{
 #ifdef DEBUG
   char toto2[80];
#endif

#if PROFILING
//		TIM_FRT_SET_16(0);
		line = bcountdiff = bcount = bcNum = 0;
#endif

	if(sms.paused && first==2)
	{
//			Cz80_Clear_NMI(&Cz80_struc);
//			Cz80_Set_NMI(&Cz80_struc);
			z80_cause_NMI();

			//Cz80_Exec(&Cz80_struc,1);
			//z80_emulate(1);
//			Cz80_Clear_NMI(&Cz80_struc);
//		Cz80_Clear_NMI(&Cz80_struc);
			first = 0;
	}

#ifdef DEBUG
   typedef enum {
      Z80_REG_AF=0,
      Z80_REG_BC,
      Z80_REG_DE,
      Z80_REG_HL,
      Z80_REG_IX,
      Z80_REG_IY,
      Z80_REG_PC,
      Z80_REG_SP,
      Z80_REG_AF2,
      Z80_REG_BC2,
      Z80_REG_DE2,
      Z80_REG_HL2,
      Z80_REG_IFF1,        /* boolean - 1 or 0 */
      Z80_REG_IFF2,        /* boolean - 1 or 0 */
      Z80_REG_IR,
      Z80_REG_IM,          /* 0, 1, or 2 */
      Z80_REG_IRQVector,   /* 0x00 to 0xff */
      Z80_REG_IRQLine,     /* boolean - 1 or 0 */
      Z80_REG_Halted       /* boolean - 1 or 0 */
   } z80_register;
#endif

    for(vdp.line = 0; vdp.line < 262; vdp.line++)
	{
/*
             sprintf(toto2,"af%xhl%x pc%x",  
                            z80_get_reg(0),
                            z80_get_reg(2)
							,z80_get_reg(6)
//							,z80_get_reg(Z80_REG_SP)
							//,z80_get_reg(Z80_REG_IR)
				 );
*/
		z80_emulate(228);
/*
#if PROFILING
				bcount += TIM_FRT_GET_16();
#endif	
*/

		if 	(vdp.line < 0x10 && (vdp.reg[0] & 0x40))
			ls_tbl[vdp.line] = (-47<<16);
		else
			if (vdp.line < 0xC0)
				ls_tbl[vdp.line] = scroll_x-(47<<16);

#if PROFILING
		TIM_FRT_SET_16(0);
#endif
		vdp_run();
//if(vb==1) SPR_WaitEndSlaveSH();
//SPR_RunSlaveSH((void*)vdp_run, &vdp);
//			vb=1;
//vdp_run(&vdp);


#ifdef PROFILING
				bcount  += TIM_FRT_GET_16();
				bcNum++;
				line++;
				bcount2[line] = (int)(TIM_FRT_CNT_TO_MCR(bcount))/bcNum;
				if(line>=21) line=0;
#endif	


	}



 #ifdef DEBUG
              sprintf(toto2,"%x bc%x de%x hl%x pc%x sp%x",  z80_get_reg(Z80_REG_AF),
							z80_get_reg(Z80_REG_BC),
							z80_get_reg(Z80_REG_DE),
                            z80_get_reg(Z80_REG_HL),
							z80_get_reg(Z80_REG_PC),
							z80_get_reg(Z80_REG_SP));

			FNT_Print256_2bpp((volatile Uint8 *)SCL_VDP2_VRAM_A1,
						(Uint8 *)toto2,0,30);

             sprintf(toto2,"ix%x iy%x af2%x",  z80_get_reg(Z80_REG_IX),
							z80_get_reg(Z80_REG_IY),
							z80_get_reg(Z80_REG_AF2));

			FNT_Print256_2bpp((volatile Uint8 *)SCL_VDP2_VRAM_A1,
						(Uint8 *)toto2,0,50);
#endif 
//VBT 11/11/2006 à remettre si probleme
//	PER_GetPort(__port);	
}

void sms_init(void)
{
	z80_init();
	sms_reset();
}

int sms_irq_callback(int param)
{
    return (0);
}
