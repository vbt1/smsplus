#include "sega_gfs.h"
#ifdef DEBUG
//#include "string.h"
#endif

#include "shared.h"
//#include "saturn.h"

//#define  ACTION_REPLAY

#ifndef ACTION_REPLAY
//#define	MAX_FILE		384
#define MAX_OPEN        8 //a ne pas modifier
#define MAX_DIR         512//384
#else
//#define	MAX_FILE		1
#define MAX_OPEN        2
#define MAX_DIR         1
#endif
//#define DEBUG 1
#define GFS_ON 1

//extern t_cart cart;
#ifndef ACTION_REPLAY
extern unsigned int file_max;
GfsDirName dir_name[MAX_DIR];
#endif

//#define VBT

#ifdef VBT
#define SYS_CDINIT1(i) \
((**(void(**)(int))0x60002dc)(i))


#define SYS_CDINIT2() \
((**(void(**)(void))0x600029c)())
#endif

//int LoadFile(char *name, Uint8 *location, Uint32 size);
//00200000H
//--------------------------------------------------------------------------------------------------------------------------------------


void InitCD()
{
	Uint32 lib_work[GFS_WORK_SIZE(MAX_OPEN) / sizeof(Uint32)];
	GfsDirTbl dirtbl; 

#ifndef ACTION_REPLAY

#ifdef VBT
CdUnlock();
#endif

//	Sint32 ret;
 
	CDC_CdInit(0x00,0x00,0x05,0x0f);
    GFS_DIRTBL_TYPE(&dirtbl) = GFS_DIR_NAME;
    GFS_DIRTBL_DIRNAME(&dirtbl) = dir_name;
    GFS_DIRTBL_NDIR(&dirtbl) = MAX_DIR;
    GFS_Init(MAX_OPEN, lib_work, &dirtbl);
#endif	    
}
//--------------------------------------------------------------------------------------------------------------------------------------
void ChangeDir(char *dirname)
{
#ifndef ACTION_REPLAY	
    Sint32 fid;
	GfsDirTbl dirtbl; 
	
    fid = GFS_NameToId((Sint8 *)dirname);

	GFS_DIRTBL_TYPE(&dirtbl) = GFS_DIR_NAME;
	GFS_DIRTBL_DIRNAME(&dirtbl) = dir_name;
	GFS_DIRTBL_NDIR(&dirtbl) = MAX_DIR;

	for (;;) {
	    file_max = GFS_LoadDir(fid, &dirtbl)-2;
		if ( file_max >= 0) {
			break;
		}
	}
GFS_SetDir(&dirtbl) ;
//	for (;;) {
//		if (GFS_SetDir(&dirtbl) == 0) {
//			break;
//		}
//	}
#endif	
}
//--------------------------------------------------------------------------------------------------------------------------------------
int LoadFile(char *name, Uint8 *location, Uint32 size)
{
#ifndef ACTION_REPLAY	
	Sint32 		fid, i;

	for (i = 0; i < 10; i++) 
	{
		fid = GFS_NameToId(name);
		if (fid >= 0) {
			GFS_Load(fid, 0, location, size);
			return 0;
		}
	}
	return -1;
#else
    return 0;
#endif	
}
//--------------------------------------------------------------------------------------------------------------------------------------
Sint32 GetFileSize(int file_id)
{
#ifndef ACTION_REPLAY	
	GfsHn gfs;
    Sint32 sctsize, nsct, lastsize;
    
    gfs = GFS_Open(file_id);
    GFS_GetFileSize(gfs, &sctsize, &nsct, &lastsize);

    GFS_Close(gfs);
//	return sctsize*nsct;
	return (sctsize*(nsct-1) + lastsize);
#else
    return 300;
#endif	
}
//--------------------------------------------------------------------------------------------------------------------------------------
#ifdef VBT

 #include "sega_cdc.h"

int CdUnlock (void)
{

Sint32 ret;
CdcStat stat;
volatile int delay;
unsigned int vbt=0;
SYS_CDINIT1(3);

SYS_CDINIT2();

do {

for(delay = 100000; delay; delay--);

ret = CDC_GetCurStat(&stat);
} while ((ret != 0) || (CDC_STAT_STATUS(&stat) == 0xff));


return (int) CDC_STAT_STATUS(&stat);

}



#endif
