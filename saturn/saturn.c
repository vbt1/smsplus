#define SBL_ON
#include    <machine.h>
#ifndef ACTION_REPLAY
#include    <sega_gfs.h>
#endif
#define CED 1

//#define PROFILING 1
//#define OLD_SOUND 1
#define		_SPR3_
#include "saturn.h"
#define titi
#define DMAOFF
#define ss_font SCL_VDP2_VRAM_A1
#define FONT_ADDR 0x25e3F000
//#define ACTION_REPLAY 1
#define FONT 1
#define SPRITE 1
extern byte *cache;
extern unsigned int first;
unsigned int sound=0;
unsigned int file_max=0;
unsigned int vsynch=0;
unsigned int play=0;
#ifndef ACTION_REPLAY
int file_id		= 4; 
#endif
#undef  SCL_MAXLINE
#undef  SCL_MAXCELL
#define	SCL_MAXLINE	 256
#define	SCL_MAXCELL	64
#define OPEN_CSH_VAR(a) (((int)&a | 0x20000000))
extern Uint32   _bstart, _bend;
//Uint16	SclLengthLsTbl;
Uint32	SclAddrLsTbl[2];
Uint16	SclProcess = 0;
extern Fixed32	ls_tbl[3 * SCL_MAXLINE];
SclSysreg	Scl_s_reg;

#ifdef titi
SclOtherPriRegister  SclOtherPri;
SclSpPriNumRegister  SclSpPriNum;
SclBgPriNumRegister  SclBgPriNum;
SclPriBuffDirtyFlags SclPriBuffDirty;
SclDataset	Scl_d_reg;
SclNorscl	Scl_n_reg;
//SclWinscl	Scl_w_reg;
//VBT passage en satic peut poser problËme ?
static Uint32	SclColRamAlloc256[8];
#endif

#ifdef FONT
unsigned int frame_x=0;
unsigned int frame_y=0;
unsigned int frame_displayed=0;
unsigned int delta=0;
unsigned int hz=0;
 int vb=0;
#endif
trigger_t	pltrigger[2],pltriggerE[2];
Uint16 pad_asign[]={
PER_DGT_U,PER_DGT_D,PER_DGT_R,PER_DGT_L,PER_DGT_A,PER_DGT_B,
PER_DGT_C,PER_DGT_S,PER_DGT_X,PER_DGT_Y,PER_DGT_TR,PER_DGT_TL,
};
#define CED 1
#ifdef SBL_ON
volatile SysPort		*__port;;
Uint8 *aVRAM;
Uint16 *colAddr; //sprite color
Uint16 *colBgAddr; // color
SprSpCmd  smsSprite[64+3];
extern int scroll_x,scroll_y;
extern t_sms sms;
SclLineparam lp;
//-------------------------------------------------------------------------------------------------------------------------------------
void	UsrVblankIn( void )
{
#ifdef FONT
	char xx[10];
	PER_GetPort(__port);	
#endif
	SCL_ScrollShow();
	
#ifndef ACTION_REPLAY
	if(play)
	{
#ifdef SOUND
#ifndef CED
if(sound)	PCM_MeVblIn();
#endif
#endif

#endif
#ifdef FONT
		frame_y++;
		if(frame_y==hz)
		{
				if(frame_displayed!=frame_x)
				{
					sprintf(xx,"%03d    ",frame_x);
					FNT_Print256_2bpp((volatile Uint8 *)SCL_VDP2_VRAM_A1,(Uint8 *)xx,90,2);
					frame_displayed = frame_x;
				}
				frame_y=frame_x=0;
		}
#endif
#ifndef ACTION_REPLAY
	}
#endif
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void	SetVblank( void ){
	
	INT_ChgMsk(INT_MSK_NULL,INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT);
	INT_SetScuFunc(INT_SCU_VBLK_IN,UsrVblankIn);
	INT_SetScuFunc(INT_SCU_VBLK_OUT,update_input);
	INT_ChgMsk(INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT,INT_MSK_NULL);
	__port = PER_OpenPort();
}
//-------------------------------------------------------------------------------------------------------------------------------------
#endif
void main(void)
{
	Uint8	*dst;
    Uint16  loop;	
    for (dst = (Uint8 *)&_bstart; dst < (Uint8 *)&_bend; dst++)					*dst = 0;
    for (dst = (Uint8 *)SystemWork, loop = 0; loop < SystemSize; loop++)	*dst = 0;
	SYS_CHGSYSCK(1);//28mhz
	set_imask(0); 
	VDP2_InitVRAM();   
	ss_main();
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void	_spr2_initialize( void )
{
	set_imask(0);
    SCL_SET_N0PRIN(5);
    SCL_SET_N1PRIN(7);
    SCL_SET_S0PRIN(6);
    SCL_ParametersInit();
    SCL_SET_SPTYPE(SCL_TYPE0);
    SCL_SET_SPCLMD(SCL_PALETTE);
    SCL_SET_SPWINEN(SCL_SP_WINDOW);
	SPR_Initial(&aVRAM);
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void initScrolling()
{
    SCL_InitLineParamTb(&lp);
    lp.line_addr=SCL_VDP2_VRAM_B0+0x4000;
	SclAddrLsTbl[0] = lp.line_addr+0x40;
	SclAddrLsTbl[1] = (Uint32 )&ls_tbl[0];
	Scl_n_reg.linecontrl = (lp.h_enbl << 1) & 0x0002;
    lp.interval=0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void ss_main(void)
{
#ifndef ACTION_REPLAY
	InitCD();
#endif
	_spr2_initialize();
	initSprites(256+48-1,192+16-1,256-1,192-1,48,16);
	initScrolling();
#ifdef CED	
	sn76496_init();
#endif	
	SetVblank();

	colBgAddr		= (Uint16*)SCL_AllocColRam(SCL_NBG0,OFF);
	colAddr			= (Uint16*)SCL_AllocColRam(SCL_SPR,OFF);
	SCL_SetColRamOffset(SCL_NBG1,16,ON);
	(Uint16*)SCL_AllocColRam(SCL_NBG1,OFF);
#ifdef FONT
	unsigned short palette[8]=
	{
	RGB( 164>>3, 181>>3, 197>>3),RGB( 0,0,0 ),RGB( 164>>3, 181>>3, 197>>3),RGB( 214>>3, 230>>3, 247>>3 ),
	RGB( 164>>3, 181>>3, 197>>3),RGB( 0,0,0 ),RGB( 164>>3, 181>>3, 197>>3),RGB( 214>>3, 230>>3, 247>>3 ),
	};
	SCL_SetColRam(SCL_NBG1,0,8,palette);
#endif
	(*(Uint16 *)0x25F8009A) = 0x0003; 
	(*(Uint16 *)0x25F80020) = 0x0303;

#ifndef OLD_SOUND
	CSH_Init(CSH_4WAY);
	SPR_InitSlaveSH();
#endif
	hz = get_hz();
	displayMenu();
	drawWindow(32,192,192,14,52);
	*(unsigned int*)OPEN_CSH_VAR(delta) = 0;

	*(Uint16 *)0x25E00000=RGB( 0, 0, 0 );//palette2[0];
    test2();
}

//-------------------------------------------------------------------------------------------------------------------------------------
#if PROFILING
extern long bcount2[21];
#endif

byte update_input(void)
{
	unsigned int i=0,k;
	byte temp = 0xFF;
	SysDevice	*device;
//	__port = PER_OpenPort();
//PER_GetPort(__port);	
	if(( device = PER_GetDeviceR( &__port[0], 0 )) != NULL )
	{
		pltriggerE[0] = pltrigger[0];
		pltrigger[0] = PER_GetTrigger( device );
		pltriggerE[0] = (pltrigger[0]) ^ (pltriggerE[0]);
		pltriggerE[0] = (pltrigger[0]) & (pltriggerE[0]);

		for(i=0;i<6;i++)
		{
			if((pltrigger[0] & pad_asign[i])!=0)
//			if((pltriggerE[0] & pad_asign[i])!=0)
			{
				switch(pltrigger[0] & pad_asign[i] )
				{
					case PER_DGT_U: temp &= ~0x01; break;
					case PER_DGT_D: temp &= ~0x02; break;
					case PER_DGT_L: temp &= ~0x04; break;
					case PER_DGT_R: temp &= ~0x08; break;
					case PER_DGT_A: temp &= ~0x10; break;
					case PER_DGT_B: temp &= ~0x20; break;
					default: 	break;
				}
			}
		}
		for(;i<12;i++)
		{
//			if((pltrigger[0] & pad_asign[i])!=0)
			if((pltriggerE[0] & pad_asign[i])!=0)
			{
				switch(pltriggerE[0] & pad_asign[i] )
				{
					case PER_DGT_C:
						if (!sms.paused)	{ sms.paused = 1; first = 2;}
						else{ sms.paused = 0;}
						temp = 0xFF;
					break;
					case PER_DGT_X:


#ifdef PROFILING
	for (k=0;k<21 ;k++)
	{
		char toto2[50];
		sprintf(toto2,"%08d", bcount2[k]);
		FNT_Print256_2bpp((volatile Uint8 *)SCL_VDP2_VRAM_A1,(Uint8 *)toto2,0,k*10);
	}

#else

#ifdef SOUND
						if(sound==1)
						{
							sound=0;
//							SYS_Exit(0);
#ifndef CED
							PCM_MeStop(pcm);
#endif							
//							pcm_PauseOn(pcm);
						}
						else
						{
							sound=1;
//							pcm_PauseOn(pcm);
#ifndef CED
							PCM_MeSetLoop(pcm, SAMPLE);
							PCM_MeStart(pcm);
#endif							
						}
#endif

#endif
					break;

					case PER_DGT_Y:
						if(vsynch==1)vsynch=0;
						else				vsynch=1;
					break;

#ifndef ACTION_REPLAY
					case PER_DGT_S: play=0; break;
					case PER_DGT_TR:
					if (file_id<=file_max)	file_id++;
					else							file_id=2;
#ifdef FONT
						FNT_Print256_2bpp((volatile Uint8 *)SCL_VDP2_VRAM_A1,
						(Uint8 *)"            ",26,2);
						FNT_Print256_2bpp((volatile Uint8 *)SCL_VDP2_VRAM_A1,
						(Uint8 *)GFS_IdToName(file_id),26,2);					
#endif
					break;

					case PER_DGT_TL:
					if (file_id>2)	file_id--;
					else				file_id=file_max+1;
#ifdef FONT
						FNT_Print256_2bpp((volatile Uint8 *)SCL_VDP2_VRAM_A1,
						(Uint8 *)"            ",26,2);
						FNT_Print256_2bpp((volatile Uint8 *)SCL_VDP2_VRAM_A1,
						(Uint8 *)GFS_IdToName(file_id),26,2);
#endif
#endif
				    default:
					break;
				}
			}
		}
	}
	else	pltrigger[0] = pltriggerE[0] = 0;

	if(( device = PER_GetDeviceR( &__port[1], 0 )) != NULL )
	{
//		pltriggerE[1] = pltrigger[1];
		pltrigger[1]  = PER_GetTrigger( device );
//		pltriggerE[1] = (pltrigger[1]) ^ (pltriggerE[1]);
//		pltriggerE[1] = (pltrigger[1]) & (pltriggerE[1]);

		if((pltrigger[1] & PER_DGT_U)!=0)  temp &= ~0x40;
		if((pltrigger[1] & PER_DGT_D)!=0)  temp &= ~0x80;	
	}
	return temp;
}

byte update_input2(void)
{
	unsigned int i=0;
	byte temp = 0xFF;
	SysDevice	*device;

	if(( device = PER_GetDeviceR( &__port[1], 0 )) != NULL )
	{
	//	pltriggerE[1] = pltrigger[1];
		pltrigger[1]  = PER_GetTrigger( device );
	//	pltriggerE[1] = (pltrigger[1]) ^ (pltriggerE[1]);
	//	pltriggerE[1] = (pltrigger[1]) & (pltriggerE[1]);

		for(i=0;i<7;i++)
		{
			if((pltrigger[1] & pad_asign[i])!=0)
			{
				switch(pltrigger[1] & pad_asign[i] )
				{
					case PER_DGT_L: temp &= ~0x01; break;
					case PER_DGT_R: temp &= ~0x02; break;
					case PER_DGT_A: temp &= ~0x04; break;
					case PER_DGT_B: temp &= ~0x08; break;
					default: 	break;
				}
			}
		}
	}
	else pltrigger[1] = 0; //pltriggerE[1] = 0;
	return ((temp & 0x3F) | (sms.port_3F & 0xC0));
}
//---------------------------------------------------------------------------------------------------------------
/* Load a game image to run */
static void load_rom(void)
{
	long fileSize;
#ifndef ACTION_REPLAY
	fileSize = GetFileSize(file_id);

// VBT 11/11/2011 : ‡ remettre si pas assez de mÈmoire
	if (fileSize <0x80000)
//		game = malloc(fileSize);
//		game = (byte *) 0x060B9000; // bon parametre
//		game = (byte *) 0x060B9000;
		game = (byte *) 0x060BD000;
//		game = (byte *) 0x00200000;        
// VBT 11/11/2011 : ‡ remettre si pas assez de mÈmoire

	else if (fileSize == 0x80000)
//		game = (byte *) 0x06079000; // bon parametre
//		game = (byte *) 0x0607A000;
		game = (byte *) 0x0607D000;
	else
	{
//		game = (byte *) 0x00202000;
		game = (byte *) 0x00200000;
	}
#else
	fileSize = sizeof(game);
#endif

	make_name_lut(fileSize);
	cart.rom	 = &game[0];
	cart.pages	 = fileSize /0x4000;
	// vbt 15/05/2008 : exophase :	 data & cart.pages, and set cart.pages to one less than you are
	// kills stree fighter 2
	//cart.pages = (fileSize /0x4000)-1;

#ifndef ACTION_REPLAY
	fileSize = GetFileSize(file_id);
//	if (LoadFile(GFS_IdToName(file_id), cart.rom,  fileSize)!=0) //taille /1600 //
	if (GFS_Load(file_id, 0, cart.rom, fileSize)<0)
	#ifdef FONT
		FNT_Print256_2bpp((volatile Uint8 *)SCL_VDP2_VRAM_A1,
			(Uint8 *)"Read pb:",6,96);
	#else
		exit(0);
	#endif

#else
		fileSize = sizeof(game);
//    file_id    = 1;
//    file_max   = 30; 
#endif

    fileSize = sizeof(game);
}
//---------------------------------------------------------------------------------------------------------------
static void test2()
{
	unsigned int i;
#ifndef ACTION_REPLAY
	while (1)
	{
//		‡ remettre __port = PER_OpenPort();
		play=1;

		while (play)
//while(1)
#else
//	init_machine();
//	while(1)
#endif
	{
		*(Uint16 *)0x25E00000 = colBgAddr[0]; // set bg_color

		sms_frame(0);
		
#ifdef OLD_SOUND //
		if(sound)
		{
#ifndef CED			
			PSG_Update(&g_movie_buf[delta],  128);
	//		SN76496Update(&g_movie_buf[delta],  128);
	//		titi2++;
			delta+=256; // DOIT etre deux fois la taille copiee

			//if (titi2==hz)
	//		if(delta>0x1200)
			if(delta>=SAMPLE*2)//256*hz)
				delta=0;
			PCM_Task(pcm);
#endif	

			



		}
#else
		if(sound)
		{
#ifndef CED				
			if(vb==1) SPR_WaitEndSlaveSH();
			SPR_RunSlaveSH((PARA_RTN*)sh2slave, NULL);
#endif			
			vb=1;
		}
#endif

		SCL_Open();
		SCL_SetLineParam2(&lp);
		Scl_n_reg.n0_move_y =  scroll_y-(16<<16) ;
		SCL_Close();

		if (vsynch)
			if(frame_x>=frame_y)
				wait_vblank();

#ifdef FONT
		frame_x++;
#endif
	}

	#ifndef ACTION_REPLAY
	for (i=0;i<256 ; i++ )
	{
		colAddr[i]=colBgAddr[i]=RGB( 0, 0, 0 );//palette2[0];
	}
	*(Uint16 *)0x25E00000=RGB( 0, 0, 0 );//palette2[0];

	for(i=0;i<0x10000;i++)
		aVRAM[0x880+i]=0x00;

	memset((Uint8 *)SCL_VDP2_VRAM_B1,0,0x10000);
	memset((Uint8 *)SCL_VDP2_VRAM_B0,0,0x2000);
	memset(vdp.vram,0,sizeof(vdp.vram));
	scroll_x= scroll_y = 0;

    for(vdp.line = 0; vdp.line < 262; vdp.line ++)
		ls_tbl[vdp.line+15]= scroll_x-(47<<16);

	SCL_Open();
	SCL_SetLineParam2(&lp);
//	SCL_SetLineParam(&lp);
	Scl_n_reg.n0_move_y =  scroll_y-(16<<16) ;
	SCL_Close();

	sms_reset();
	load_rom();
// ‡ remettre	__port = PER_OpenPort();
//	system_reset();
//	sms_reset();
	system_init(0);
	
}
//	system_init(0);	   
//	test2();

#else
		play=1;
#endif

}
//--------------------------------------------------------------------------------------------------------------
#ifndef OLD_SOUND

#ifndef CED	
static void sh2slave()
{
//	CSH_AllClr();
	unsigned int deltaSlave;
	deltaSlave =  *(unsigned int*)OPEN_CSH_VAR(delta);
	PSG_Update(&g_movie_buf[deltaSlave],  128);
	deltaSlave+=256; 
	if(deltaSlave>=SAMPLE*2)//256*hz)
		deltaSlave=0;
	PCM_Task(pcm);

	*(unsigned int*)OPEN_CSH_VAR(delta) = deltaSlave;
}
#endif
#endif
//--------------------------------------------------------------------------------------------------------------
static void VDP2_InitVRAM(void)
{
	Uint32	loop = 0;
	while (loop < 0x40000)
	{
		*((Uint32 *) (SCL_VDP2_VRAM_A0 + loop)) = 0;
		*((Uint32 *) (SCL_VDP2_VRAM_B0 + loop)) = 0;
		loop += 4;
	}
}
//--------------------------------------------------------------------------------------------------------------
static void displayMenu(void)
{
//    SclConfig	config;
// **29/01/2007 : VBT sauvegarde cycle patter qui fonctionne jusqu'‡ maintenant

	Uint16	CycleTb[]={
		  // VBT 04/02/2007 : cycle pattern qui fonctionne just test avec des ee
		0xff5e, 0xffff, //A1
		0xffff, 0xffff,	//A0
		0x04ee, 0xffff,   //B1
		0xffff, 0xffff  //B0
	};
 	SclConfig	scfg;
	cache     = (Uint8 *)SCL_VDP2_VRAM_B1;
//	SCL_InitConfigTb(&scfg);
	scfg.dispenbl		= ON;
	scfg.charsize		= SCL_CHAR_SIZE_1X1;//OK du 1*1 surtout pas toucher
	scfg.pnamesize	= SCL_PN1WORD;
	scfg.flip				= SCL_PN_10BIT;
	scfg.platesize	= SCL_PL_SIZE_1X1;
	scfg.coltype		= SCL_COL_TYPE_16;
	scfg.datatype	= SCL_CELL;
	scfg.patnamecontrl =  0x000c;// VRAM B1 ÇÃÉIÉtÉZÉbÉg 
	scfg.plate_addr[0] = SCL_VDP2_VRAM_B0;
	SCL_SetConfig(SCL_NBG0, &scfg);
/********************************************/	

//	SCL_InitConfigTb(&scfg);
	scfg.dispenbl 	 = ON;
	scfg.bmpsize 		 = SCL_BMP_SIZE_512X256;
	scfg.coltype 		 = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype 	 = SCL_BITMAP;
	scfg.mapover       = SCL_OVER_0;
	scfg.plate_addr[0] = SCL_VDP2_VRAM_A1;
	SCL_SetConfig(SCL_NBG1, &scfg);
	SCL_SetCycleTable(CycleTb);	
	PER_SMPC_RES_ENA();// ÉäÉZÉbÉgÉ{É^ÉìóLå¯ 
	SCL_Open();
//	SCL_MoveTo(FIXED(0), FIXED(0),0);
	//SCL_MoveTo(FIXED(0), FIXED(0),0);
	Scl_n_reg.n1_move_x = 0;
	Scl_n_reg.n1_move_y = 0;
	SCL_Close();

#ifdef SOUND

#ifndef CED
	sndInit();
	PCM_MeInit();
	PcmCreatePara	para;
	PcmInfo 		info;
	PcmStatus	*st;
	static PcmWork g_movie_work;

//#define	SDDRV_SIZE	26624 //0x7000
//#define	SDDRV_ADDR	0x00200000//0x6080000

	PCM_PARA_WORK(&para) = (struct PcmWork *)&g_movie_work;
	PCM_PARA_RING_ADDR(&para) = (Sint8 *)g_movie_buf;
	PCM_PARA_RING_SIZE(&para) = RING_BUF_SIZE;
	PCM_PARA_PCM_ADDR(&para) = PCM_ADDR;
	PCM_PARA_PCM_SIZE(&para) = PCM_SIZE;

 	st = &g_movie_work.status;

	memset((Sint8 *)g_movie_buf,0,SAMPLE*4);

	st->need_ci = PCM_ON;
	PCM_INFO_FILE_TYPE(&info) = PCM_FILE_TYPE_NO_HEADER;			
	PCM_INFO_DATA_TYPE(&info)=PCM_DATA_TYPE_RLRLRL;//PCM_DATA_TYPE_LRLRLR;
	PCM_INFO_FILE_SIZE(&info) = SAMPLE*2;//0x4000;//214896;
	PCM_INFO_CHANNEL(&info) = 0x01;
	PCM_INFO_SAMPLING_BIT(&info) = 16;
	PCM_INFO_SAMPLING_RATE(&info)	= SAMPLE;//30720L;//44100L;
	PCM_INFO_SAMPLE_FILE(&info) = SAMPLE*2;//30720L;//214896;
			
	pcm = createHandle(&para);

	PCM_SetPcmStreamNo(pcm, 0);


	PCM_SetInfo(pcm, &info);
	PCM_ChangePcmPara(pcm);

	//PCM_SetLoop(pcm, 30720L);
	PCM_MeSetLoop(pcm, SAMPLE);

	if (pcm == NULL) return;
#endif	
	
#endif

#ifndef ACTION_REPLAY
//		GFS_Load(GFS_NameToId("FONT.BIN"),0,(void *) FntAsciiFontData2bpp,1600);
	GFS_Load(GFS_NameToId("FONT.BIN"),0,(void *)  FONT_ADDR,1600);
	ChangeDir("GAMES");
	ClrVram((volatile Uint8 *)SCL_VDP2_VRAM_A1);
#endif
#ifdef SOUND
	sound = 1;
#ifndef CED	
	PCM_Start(pcm);
#endif	
#endif
	vsynch = 1;
	memset(&aVRAM[0x880],0,0x20000);
	memset(cache,0,sizeof(cache));
	memset((Uint16 *)SCL_VDP2_VRAM_B0,0,0x1000);
	
	make_cram_lut();
	make_bp_lut();
	sms_reset();
	load_rom();

	system_init(0);	
}

#ifndef ACTION_REPLAY

static void	ClrVram(volatile Uint8 *buff)
{
	Uint32	i;
	for(i=0;i<0x10000;i++)	buff[i]=0x00;
}

#endif		

#ifdef titi
#define	REGADDR	0x25F80000

//-------------------------------------------------------------------------------------------------------------------------------------
static void SCL_ParametersInit(void)
{
//	Uint16	i;
/*
 *	System Registers Area Initialization
 */
	Scl_s_reg.tvmode = 0x8001; /* HREZ = B'001(352 pixels) the others = 0 */
	Scl_s_reg.ramcontrl = 0x1000; /* color 16bit mode; VRAM A, B banked */
/*
 *	Normal Scroll Registers Area Initialization
 */
	Scl_n_reg.n0_move_x = 0;
	Scl_n_reg.n0_move_y = 0;
	Scl_n_reg.n0_delta_x = FIXED(1);
	Scl_n_reg.n0_delta_y = FIXED(1);
	Scl_n_reg.n1_move_x = 0;
	Scl_n_reg.n1_move_y = 0;
	Scl_n_reg.n1_delta_x = FIXED(1);
	Scl_n_reg.n1_delta_y = FIXED(1);
	Scl_n_reg.n2_move_x = 0;
	Scl_n_reg.n2_move_y = 0;
	Scl_n_reg.n3_move_x = 0;
	Scl_n_reg.n3_move_y = 0;
	Scl_n_reg.zoomenbl = 0;
	Scl_n_reg.linecontrl = 0;
	Scl_n_reg.celladdr = 0;
	Scl_n_reg.lineaddr[0] = 0;
	Scl_n_reg.lineaddr[1] = 0;
	Scl_n_reg.linecolmode = 0;
	Scl_n_reg.backcolmode = 0;

	if(SclProcess == 0)	SclProcess = 1;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void SCL_CopyReg()
{
	Uint16 *regaddr = (Uint16 *)REGADDR;
	memcpyl(&regaddr[0x38], &Scl_n_reg, sizeof(SclNorscl));

	if(!play)
	{
		regaddr[0] = Scl_s_reg.tvmode;		/* add				by C.Y	*/
		regaddr[1] = Scl_s_reg.extenbl;		/* add				by C.Y	*/
		regaddr[3] = Scl_s_reg.vramsize;		/* add				by C.Y	*/

		memcpyw(&regaddr[7] , &Scl_s_reg.ramcontrl , 26 );
		memcpyl(&regaddr[0x14], &Scl_d_reg, sizeof(SclDataset));
//	    memcpyw(&regaddr[0x60], &Scl_w_reg, sizeof(SclWinscl));

		memcpy((SclOtherPriRegister *)0x25F800E0, &SclOtherPri, sizeof(SclOtherPri));
		memcpy((SclSpPriNumRegister *)0x25F800F0, &SclSpPriNum, sizeof(SclSpPriNum));
		memcpy((SclBgPriNumRegister *)0x25F800F8, &SclBgPriNum, sizeof(SclBgPriNum));
//		memcpyw((SclBgColMixRegister *)0x25F80108, &SclBgColMix, sizeof(SclBgColMix));
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void SCL_InitLineParamTb(SclLineparam *lp)
{
	Uint16	i;

	lp->h_enbl = ON;
	lp->v_enbl = OFF;
	lp->delta_enbl = OFF;
	lp->cell_enbl = OFF;
	lp->line_addr = SCL_VDP2_VRAM_B0+0x4000;
	lp->cell_addr = 0x00;
	lp->interval  = 0;

	for(i = 0; i< SCL_MAXLINE; i++) {
//	for(i = 0; i< 192; i++) {
		if(i < SCL_MAXCELL) {
			lp->cell_tbl[i] = 0;
		}
		lp->line_tbl[i].h  = FIXED(0);
		lp->line_tbl[i].v  = FIXED(i);
		lp->line_tbl[i].dh = FIXED(1);
//		lp->line_tbl[i].h  = (0<<16);
//		lp->line_tbl[i].v  = (i<<16);
//		lp->line_tbl[i].dh = (1<<16);
	}

	SclAddrLsTbl[0] = lp->line_addr+0x40;
	SclAddrLsTbl[1] = (Uint32 )&ls_tbl[0];
	Scl_n_reg.linecontrl = (lp->h_enbl << 1) & 0x0002;
}
//-------------------------------------------------------------------------------------------------------------------------------------
inline void	SCL_ScrollShow(void)
{
    switch(SclProcess){
	    case 1:
		SCL_CopyReg();
		SclProcess = 0;
		break;
	    case 2:			/* line scroll setting */
		memcpy((void *)SclAddrLsTbl[0],(void *)SclAddrLsTbl[1], 0x300);
		SCL_CopyReg();
		SclProcess = 0;
		break;
		default:
		break;
    }
}
//-------------------------------------------------------------------------------------------------------------------------------------
void SCL_Open(void)
{
	if(SclProcess == 1)	SclProcess = 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
void SCL_Close(void)
{
	if(SclProcess == 0)	SclProcess = 1;
}
//-------------------------------------------------------------------------------------------------------------------------------------
void	SCL_SetCycleTable(Uint16 *tp)
{
	Uint16 i;
	for(i = 0; i<8; i++){
		Scl_s_reg.vramcyc[i] = tp[i];
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
void SCL_SetConfig(Uint16 sclnum, SclConfig *scfg)
{
/*	Uint16	temp, *mapoffset, boundary, *map, *map2, shift,shift2;	*/
	Uint16	temp;
	Uint16	*mapoffset = 0;
	Uint16	*map = 0;
//	Uint16	*map2;
	Uint16	boundary = 0;
	Uint16	shift = 0;
//	Uint16	shift2 = 0;
										/* add by C.yoshida */

/*	Uint16	max, i, flip, mapoffsetW;	*/
	Uint16	max = 0;
	Uint16	i;
//	Uint16	flip;
	Uint16	mapoffsetW;
										/* add by C.yoshida */

//	map2 = 0;

//	flip = scfg->flip;				/* get flip bit*/
//	flip = (flip<<14) & 0x4000;
	switch(sclnum) {
		case SCL_NBG0:
			temp = scfg->dispenbl;		/* display enable */
			temp &= 0x0001;
			Scl_s_reg.dispenbl &= 0xfffe;
			Scl_s_reg.dispenbl |= temp;

			temp = scfg->charsize;		/* char size */
			temp &= 0x0001;
			Scl_d_reg.charcontrl0 &= 0xfffe;
			Scl_d_reg.charcontrl0 |= temp;

			Scl_d_reg.patnamecontrl[0] = scfg->patnamecontrl;

			temp = scfg->pnamesize;		/* pattern name size */
			temp = (temp <<15) & 0x8000;
			Scl_d_reg.patnamecontrl[0] &= 0x7fff;
			Scl_d_reg.patnamecontrl[0] |= temp;

//			Scl_d_reg.patnamecontrl[0] &= 0xbfff;	/* flip bit set */
//			Scl_d_reg.patnamecontrl[0] |= flip;	/* flip bit set */

			temp = scfg->platesize;		/* plate size */
			temp &=  0x0003;
			Scl_d_reg.platesize &= 0xfffc;
			Scl_d_reg.platesize |= temp;

			temp = scfg->bmpsize;		/* bitmap size */
			temp = (temp << 2) & 0x000c;
			Scl_d_reg.charcontrl0 &= 0xfff3;
			Scl_d_reg.charcontrl0 |= temp;

			temp = scfg->coltype;		/* color type */
			temp = (temp <<4) & 0x0070;
			Scl_d_reg.charcontrl0 &= 0xff8f;
			Scl_d_reg.charcontrl0 |= temp;

			temp = scfg->datatype;		/* data type */
			temp = (temp <<1) & 0x0002;
			Scl_d_reg.charcontrl0 &= 0xfffd;
			Scl_d_reg.charcontrl0 |= temp;

			Scl_d_reg.mapoffset0 &= 0xfff0;	/* Init map offset */ 
			mapoffset = &Scl_d_reg.mapoffset0; /* map offset */ 
			shift = 0;
			map = &Scl_d_reg.normap[0];
			max = 2;
			break;
		case SCL_NBG1:
			temp = scfg->dispenbl;		/* display enable */
			temp = (temp << 1) & 0x0002;
			Scl_s_reg.dispenbl &= 0xfffD;
			Scl_s_reg.dispenbl |= temp;

			temp = scfg->charsize;		/* char size */
			temp = (temp<<8) & 0x0100;
			Scl_d_reg.charcontrl0 &= 0xfeff;
			Scl_d_reg.charcontrl0 |= temp;

			Scl_d_reg.patnamecontrl[1] = scfg->patnamecontrl;

			temp = scfg->pnamesize;		/* pattern name size */
			temp = (temp <<15) & 0x8000;
			Scl_d_reg.patnamecontrl[1] &= 0x7fff;
			Scl_d_reg.patnamecontrl[1] |= temp;

//			Scl_d_reg.patnamecontrl[1] &= 0xbfff;
//			Scl_d_reg.patnamecontrl[1] |= flip; /* flip bit set*/

			temp = scfg->platesize;		/* plate size */
			temp = (temp << 2) &  0x000c;
			Scl_d_reg.platesize &= 0xfff3;
			Scl_d_reg.platesize |= temp;

			temp = scfg->bmpsize;		/* bitmap size */
			temp = (temp << 10) & 0x0c00;
			Scl_d_reg.charcontrl0 &= 0xf3ff;
			Scl_d_reg.charcontrl0 |= temp;

			temp = scfg->coltype;		/* color type */
			temp = (temp <<12) & 0x3000;
			Scl_d_reg.charcontrl0 &= 0xcfff;
			Scl_d_reg.charcontrl0 |= temp;

			temp = scfg->datatype;		/* data type */
			temp = (temp <<9) & 0x0200;
			Scl_d_reg.charcontrl0 &= 0xfdff;
			Scl_d_reg.charcontrl0 |= temp;

			Scl_d_reg.mapoffset0 &= 0xff0f;	/* Init map offset */ 
			mapoffset = &Scl_d_reg.mapoffset0; /* map offset */ 
			shift = 4;
			map = &Scl_d_reg.normap[2];
			max = 2;
			break;
	}
/*
 *	Set Map Address
 */

	if(scfg->datatype == SCL_BITMAP) {
		mapoffsetW = ((scfg->plate_addr[0] - SCL_VDP2_VRAM)/0x20000) & 0x0007;
		*mapoffset |= mapoffsetW << shift;
	} else {
		if( scfg->pnamesize == 1){
			if(scfg->charsize == 0) {
				boundary = 0x2000;
			} else {
				boundary = 0x800;
			}
		} else {
			if(scfg->charsize == 0) {
				boundary = 0x4000;
			} else {
				boundary = 0x1000;
			}
		}
		mapoffsetW = (0x01c0 & ((scfg->plate_addr[0] - SCL_VDP2_VRAM) / boundary))
			>> 6;
		*mapoffset |= mapoffsetW << shift;
	}

	for(i = 0; i < max; i++) {
		map[i] = (0x003f & ((scfg->plate_addr[i * 2] - SCL_VDP2_VRAM)
				/ boundary));
		temp = (0x003f & ((scfg->plate_addr[i * 2 + 1] - SCL_VDP2_VRAM)
				/ boundary)) << 8;

		map[i] |= (temp & 0x3f00);
	}
//	if(SclProcess == 0)	SclProcess = 4;
//modif VBT
	if(SclProcess == 0)	SclProcess = 1;
}
//-------------------------------------------------------------------------------------------------------------------------------------
void  SCL_SetColRam(Uint32 Object, Uint32 Index,Uint32 num,void *Color)
{
	Uint16	*ram16;
	ram16   = (Uint16 *)((Uint32)SCL_COLRAM_ADDR + ((SCL_GetColRamOffset(Object) * 0x200))) + Index;
	memcpyl(ram16,(Uint16 *)Color,num*2);
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void  SCL_SetColRamOffset(Uint32 Object, Uint32 Offset,Uint8 transparent)
{
    if(Object & SCL_SPR)	SCL_SET_SPCAOS(Offset);

    if( (Object & SCL_NBG0)) //|| (Object & SCL_RBG1))
    {
	SCL_SET_N0CAOS(Offset);
	if(transparent)	Scl_s_reg.dispenbl |= 0x0100;
	else		Scl_s_reg.dispenbl &= 0xfeff;
    }

    if( (Object & SCL_NBG1)) // || (Object & SCL_EXBG))
    {
	SCL_SET_N1CAOS(Offset);
	if(transparent)	Scl_s_reg.dispenbl |= 0x0200;
	else		Scl_s_reg.dispenbl &= 0xfdff;
    }
}
//-------------------------------------------------------------------------------------------------------------------------------------
Uint32  SCL_AllocColRam(Uint32 Surface, Uint8 transparent)
{
	Uint32	i;
	for(i=0;i<8;i++)
	{
		if(SclColRamAlloc256[i]==0)
		{
			SclColRamAlloc256[i]=Surface;
			SCL_SetColRamOffset(Surface,i,transparent);
			return(SCL_COLRAM_ADDR+(512*i));
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
// VBT passage en static
static Uint32  SCL_GetColRamOffset(Uint32 Object)
{
    switch(Object){
      case SCL_SPR:
	return(SCL_GET_SPCAOS());
	break;
      case SCL_NBG0:
//      case SCL_RBG1:
	return(SCL_GET_N0CAOS());
	break;
      case SCL_NBG1:
//      case SCL_EXBG:
	return(SCL_GET_N1CAOS());
	break;
      default:
	break;
    }
}
#endif
//-------------------------------------------------------------------------------------------------------------------------------------
static void SCL_SetLineParam2(SclLineparam *lp)
{
	Uint32	*addr;
	addr = &Scl_n_reg.lineaddr[0];
	//*addr = (lp->line_addr / 2) & 0x0007ffff;
	*addr = (lp->line_addr >>1) & 0x0007ffff;
	SclProcess = 2; //obligatoire
}
//-------------------------------------------------------------------------------------------------------------------------------------
void SPR_Initial(Uint8 **VRAM)
{
    int     iMask;
    iMask = get_imask();
    set_imask(15);                           /* interrupt disable           */

    /* change frame buffer to auto change mode  */
    SPR_WRITE_REG(SPR_W_FBCR, SPR_FBCR_AUTO );
    /* trig set to auto start drawing */
    SPR_WRITE_REG(SPR_W_PTMR, 0x0002);
    *VRAM     = (Uint8*)VRAM_ADDR;
    *(*VRAM)     = 0x80;                 /* set END command to VRAM top area */
    *((*VRAM)+1) = 0x00;
    set_imask(iMask);                    /* interrupt enable             */
}
//-------------------------------------------------------------------------------------------------------------------------------------
void SPR_SetEraseData(Uint16 eraseData, Uint16 leftX, Uint16 topY,Uint16 rightX, Uint16 botY)
{
	SPR_WRITE_REG(SPR_W_EWDR, eraseData);      /* set erase write data */

	leftX >>= 3;
	SPR_WRITE_REG(SPR_W_EWLR, (leftX << 9) + topY); /* set erase screen left top */

	rightX++;
	rightX >>= 3;
	SPR_WRITE_REG(SPR_W_EWRR, (rightX << 9) + botY);   /* set erase screen right bottom */
}
//-------------------------------------------------------------------------------------------------------------------------------------
void SND_Init(SndIniDt *sys_ini)
{
	Uint8 *adr_sys_info_tbl;
	
	/** BEGIN ****************************************************************/

    PER_SMPC_SND_OFF();                         /* ÉTÉEÉìÉhOFF               */
    POKE_W(ADR_SCSP_REG, SCSP_REG_SET); 
                                                /* SCSPã§í ⁄ºﬁΩ¿ê›íË         */

    DmaClrZero(ADR_SND_MEM, MEM_CLR_SIZE);      // DMAÉÅÉÇÉäÉ[ÉçÉNÉäÉA       

		CopyMem(ADR_SND_VECTOR,
                   (void *)(SND_INI_PRG_ADR(*sys_ini)),
                   SND_INI_PRG_SZ(*sys_ini));   // 68KÃﬂ€∏ﬁ◊—ì]ëó            

    adr_sys_info_tbl = (Uint8 *)(ADR_SND_MEM + PEEK_L(ADR_SYS_TBL +
                                 ADR_SYS_INFO));
                                                /* ºΩ√—èÓïÒ√∞ÃﬁŸ±ƒﬁ⁄ΩéÊìæ    */
    adr_host_int_work = (Uint8 *)(ADR_SND_MEM + PEEK_L(ADR_SYS_TBL +
                                  ADR_HOST_INT));
                                                
    adr_com_block = adr_host_int_work;  /* åªç›èëÇ´çûÇ›∫œ›ƒﬁÃﬁ€Ø∏±ƒﬁ⁄Ωèâä˙âª */

		CopyMem((void *)
                    (PEEK_L(adr_sys_info_tbl + ADR_ARA_ADR) + ADR_SND_MEM),
                   (void *)(SND_INI_ARA_ADR(*sys_ini)),
                   CHG_LONG(SND_INI_ARA_SZ(*sys_ini))); /* ª≥›ƒﬁ¥ÿ±œØÃﬂì]ëó  */
/* 1994/02/24 Start*/
    intrflag = 0;         /* äÑÇËçûÇ›ÉtÉâÉOÇÃèâä˙âª */
/* 1994/02/24 End */

    PER_SMPC_SND_ON();                          /* ÉTÉEÉìÉhON                */

}
//-------------------------------------------------------------------------------------------------------------------------------------
Uint8 SND_ChgMap(Uint8 area_no)
 {
/* 1994/02/24 Start */
    if(intrflag) return(SND_RET_NSET);
    intrflag = 1;
/* 1994/02/24 End */
    if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
    SET_PRM(0, area_no);                        /* ÉpÉâÉÅÅ[É^ÉZÉbÉg          */
    SET_COMMAND(COM_CHG_MAP);                   /* ÉRÉ}ÉìÉhÉZÉbÉg            */

    while(PEEK_W(adr_com_block + ADR_COM_DATA)) _WAIT_();
    if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
    SET_PRM(0, area_no);                        /* ÉpÉâÉÅÅ[É^ÉZÉbÉg          */
    SET_COMMAND(COM_CHG_MAP);                   /* ÉRÉ}ÉìÉhÉZÉbÉg            */
    while(PEEK_W(adr_com_block + ADR_COM_DATA)) _WAIT_();
    HOST_SET_RETURN(SND_RET_SET);
}

#define DMA_SCU_END     0
//-------------------------------------------------------------------------------------------------------------------------------------
static void CopyMem(void *dst, void *src, Uint32 cnt)
{
	memcpyw(dst, src, cnt);
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void DmaClrZero(void *dst, Uint32 cnt)
{
	memset(dst, 0x00, cnt);
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void sndInit(void)
{
	SndIniDt 	snd_init;
	unsigned char sound_map[]={0xFF,0xFF};

#ifndef ACTION_REPLAY
	GFS_Load(GFS_NameToId(SDDRV_NAME),0,(void *) SDDRV_ADDR,SDDRV_SIZE);
	SND_INI_PRG_ADR(snd_init) = (Uint16 *)SDDRV_ADDR;
	SND_INI_PRG_SZ(snd_init)  = (Uint16)  SDDRV_SIZE;
#else
	
	SND_INI_PRG_ADR(snd_init) = (Uint16 *)snddrv;
	SND_INI_PRG_SZ(snd_init)  = (Uint16)  sizeof(snddrv);
#endif

	SND_INI_ARA_ADR(snd_init) 	= (Uint16 *)sound_map;
	SND_INI_ARA_SZ(snd_init) 	= (Uint16)sizeof(sound_map);

	SND_Init(&snd_init);
	SND_ChgMap(0);
}
//-------------------------------------------------------------------------------------------------------------------------------------
static PcmHn createHandle(PcmCreatePara *para)
{
	PcmHn pcm;

	pcm = PCM_CreateMemHandle(para);
	if (pcm == NULL) {
		return NULL;
	}
	PCM_NotifyWriteSize(pcm, SAMPLE*2);

	return pcm;
}
//-------------------------------------------------------------------------------------------------------------------------------------
 SndRet SND_ChgPcm(SndPcmChgPrm *cprm)
 {
/* 1994/02/24 Start */
    if(intrflag) return(SND_RET_NSET);
    intrflag = 1;
/* 1994/02/24 End */
    if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
    SET_PRM(0, SND_PRM_NUM(*cprm));
    SET_PRM(1, (SND_PRM_LEV(*cprm) << 5) | ChgPan(SND_PRM_PAN(*cprm)));
    SET_PRM(2, SND_PRM_PICH(*cprm) >> 8);
    SET_PRM(3, SND_PRM_PICH(*cprm));
    SET_PRM(4, (SND_R_EFCT_IN(*cprm) << 3) | SND_R_EFCT_LEV(*cprm));
    SET_PRM(5, (SND_L_EFCT_IN(*cprm) << 3) | SND_L_EFCT_LEV(*cprm));
    SET_COMMAND(COM_CHG_PCM_PRM);               /* ÉRÉ}ÉìÉhÉZÉbÉg            */
    HOST_SET_RETURN(SND_RET_SET);
}
//-------------------------------------------------------------------------------------------------------------------------------------
static Uint8 GetComBlockAdr(void)
{
    if(*NOW_ADR_COM_DATA){              /* à»ëOÇÃÃﬁ€Ø∏Ç™à¯Ç´éÊÇËçœÇ›Ç≈Ç»Ç¢Ç©?*/
        /* éüÉRÉ}ÉìÉhÉuÉçÉbÉNÉAÉhÉåÉXê›íËèàóù ********************************/
        if(NOW_ADR_COM_DATA >= (MAX_ADR_COM_DATA - SIZE_COM_BLOCK)){
                                                    /* ç≈ëÂílÇ©?            */
            return OFF;                             /* Ãﬁ€Ø∏ãÛÇ´ñ≥Çµ      */
        }else{
            adr_com_block += SIZE_COM_BLOCK;        /* åªç›∫œ›ƒﬁÃﬁ€Ø∏∂≥›ƒ±ØÃﬂ*/
            while(NOW_ADR_COM_DATA < (MAX_ADR_COM_DATA - SIZE_COM_BLOCK)){
                if(*NOW_ADR_COM_DATA){
                    adr_com_block += SIZE_COM_BLOCK;
                }else{
                    return ON;                      /* Ãﬁ€Ø∏ãÛÇ´óLÇË         */
                }
            }
            return OFF;                             /* Ãﬁ€Ø∏ãÛÇ´ñ≥Çµ         */
        }
    }else{
        adr_com_block = adr_host_int_work;  /* Ãﬁ€Ø∏ÇÃêÊì™Ç÷              */
        while(NOW_ADR_COM_DATA < (MAX_ADR_COM_DATA - SIZE_COM_BLOCK)){
            if(*NOW_ADR_COM_DATA){
                adr_com_block += SIZE_COM_BLOCK;
            }else{
                return ON;                          /* Ãﬁ€Ø∏ãÛÇ´óLÇË         */
            }
        }
        return OFF;                                 /* Ãﬁ€Ø∏ãÛÇ´ñ≥Çµ         */
    }
}
//-------------------------------------------------------------------------------------------------------------------------------------
static Uint16 ChgPan(SndPan pan)
{
    return(((pan) < 0) ? (~(pan) + 0x10 + 1) : (pan));
}
//-------------------------------------------------------------------------------------------------------------------------------------
 SndRet SND_StartPcm(SndPcmStartPrm *sprm, SndPcmChgPrm *cprm)
 {
    if(intrflag) return(SND_RET_NSET);
    intrflag = 1;
/* 1994/02/24 End */
    if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
    SET_PRM(0, SND_PRM_MODE(*sprm) | SND_PRM_NUM(*cprm));
    SET_PRM(1, (SND_PRM_LEV(*cprm) << 5) | ChgPan(SND_PRM_PAN(*cprm)));
    SET_PRM(2, SND_PRM_SADR(*sprm) >> 8);
    SET_PRM(3, SND_PRM_SADR(*sprm));
    SET_PRM(4, SND_PRM_SIZE(*sprm) >> 8);
    SET_PRM(5, SND_PRM_SIZE(*sprm));
    SET_PRM(6, SND_PRM_PICH(*cprm) >> 8);
    SET_PRM(7, SND_PRM_PICH(*cprm));
    SET_PRM(8, (SND_R_EFCT_IN(*cprm) << 3) | SND_R_EFCT_LEV(*cprm));
    SET_PRM(9, (SND_L_EFCT_IN(*cprm) << 3) | SND_L_EFCT_LEV(*cprm));
    SET_PRM(11, 0);
    SET_COMMAND(COM_START_PCM);                 /* ÉRÉ}ÉìÉhÉZÉbÉg            */
    HOST_SET_RETURN(SND_RET_SET);
}
//-------------------------------------------------------------------------------------------------------------------------------------
 SndRet SND_StopPcm(SndPcmNum pcm_num)
 {
/* 1994/02/24 Start */
    if(intrflag) return(SND_RET_NSET);
    intrflag = 1;
/* 1994/02/24 End */
    if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
    SET_PRM(0, pcm_num);                        /* ÉpÉâÉÅÅ[É^ÉZÉbÉg          */
    SET_COMMAND(COM_STOP_PCM);                  /* ÉRÉ}ÉìÉhÉZÉbÉg            */
    HOST_SET_RETURN(SND_RET_SET);
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void wait_vblank(void)
{
     while((TVSTAT & 8) == 0);
     while((TVSTAT & 8) == 8);
}
//-------------------------------------------------------------------------------------------------------------------------------------
static unsigned int get_hz(void)
{
	if((TVSTAT & 1) == 0)
		return 60;
	else
		return 50;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void initSprites(int sx,int sy,int sx2, int sy2,int lx,int ly)
{
//    int i;
	SPR_WRITE_REG(SPR_W_TVMR, 0x0007 & SPR_TV_NORMAL);//SPR_TV_ROT8);//SPR_TV_NORMAL);
	SPR_SetEraseData( 0x0000, 0, 0, sx, sy );

	memset(smsSprite,0,sizeof(SprSpCmd)*3);
    smsSprite[0].control    = (JUMP_NEXT | FUNC_SCLIP);

    smsSprite[0].cx         = sx;
    smsSprite[0].cy         = sy;
    smsSprite[0].dx         = sx2;
    smsSprite[0].dy         = sy2;

    smsSprite[1].control    =  (JUMP_NEXT | FUNC_LCOORD);

    smsSprite[1].ax         = lx;
    smsSprite[1].ay         = ly;

    smsSprite[2].control    = (JUMP_NEXT | FUNC_UCLIP);

	smsSprite[2].ax         = sx;
	smsSprite[2].ay         = sy;

    smsSprite[3].control    = CTRL_END;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void drawWindow(unsigned  int l1,unsigned  int l2,unsigned  int l3,unsigned  int vertleft,unsigned  int vertright)
{
	Uint16 *VRAM;    
	int x,j;
    VRAM = (Uint16 *)ss_font;
// barre horizontale haut
	for( x = 0; x < l1; x++ ) // 2 lignes
    {
		for( j = 0; j < 64; j++ ) *VRAM++ = 0x2222;
	}

	for( x = 0; x < l2; x++ ) 
    {
		for( j = 0; j < vertleft			  ; j++ ) *VRAM++ = 0x2222; // barre verticale gauche
		for( j = 0; j < 128-vertleft-vertright; j++ ) *VRAM++ = 0x0000; // noir
		for( j = 0; j < vertright			  ; j++ ) *VRAM++ = 0x2222; // barre verticale droite
	}
// barre horizontale bas
	for( x = 0; x < l3; x++ ) 
    {
		for( j = 0; j < 64; j++ ) *VRAM++ = 0x2222;
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
