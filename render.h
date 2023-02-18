
#ifndef _RENDER_H_
#define _RENDER_H_

/* Used for blanking a line in whole or in part */
//#define BACKDROP_COLOR      (0x10 | (vdp.reg[7] & 0x0F))
/* Global data - used by 'vdp.c' */
extern byte vram_dirty[0x200];
extern byte is_vram_dirty;

/* Function prototypes */
void render_reset(void);
//void render_bg_sms(int line);
//void render_obj(int line);
//void render_line(int line);
//void update_cache(void);

#endif /* _RENDER_H_ */
