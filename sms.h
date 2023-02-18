
#ifndef _SMS_H_
#define _SMS_H_

//#define TYPE_OVERSEAS   (0)
#define TYPE_DOMESTIC   (1)

/* SMS context */

typedef struct
{
	byte ram[0x2000];
	byte sram[0x8000];
//#ifdef VBT
	byte fcr[4];
//#endif
	byte paused;
//	byte save;
	byte country;
	byte port_3F;
	byte port_F2;
//	byte use_fm;
	byte irq;
//	byte psg_mask;
}t_sms;

/* Global data */
// VBT 04/02/2007 passage en static 

/* Function prototypes */
void sms_frame(int skip_render);
void sms_init(void);
void sms_reset(void);
int  sms_irq_callback(int param);
#ifdef VBT
void cpu_reset(void);
#endif

#endif /* _SMS_H_ */
