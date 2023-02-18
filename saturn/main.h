#ifndef _MAIN_H_
#define _MAIN_H_
//#define		NBG1_CEL_ADR		( VDP2_VRAM_A1 + 0x02000 )
//#define		NBG1_MAP_ADR		( VDP2_VRAM_B1 + 0x12000 )
//#define		NBG1_COL_ADR		( VDP2_COLRAM + 0x00200 )
//#define		BACK_COL_ADR		( VDP2_VRAM_A1 + 0x1fffe )
#define     SystemWork      0x060ffc00              /* System Variable Address */
#define     SystemSize      (0x06100000-0x060ffc00) /* System Variable Size */

extern Uint32   _bstart, _bend;

void ss_main(void);
//void Pal2CRAM( Uint16 *Pal_Data , void *Col_Adr , Uint32 suu );
int LoadFile(char *name, Uint8 *location, Uint32 size);
void InitCD();
#endif

