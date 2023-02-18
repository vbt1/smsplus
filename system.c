#include "shared.h"
//#include "stddef.h"

void vdp_reset(void);
void sms_init(void);
//int SN76496_init(int clock,int volume,int sample_rate);
void PSG_Init(int clock, int rate);
t_cart cart;

void system_init(int rate)
{
    /* Initialize the VDP emulation */
    vdp_reset();

    /* Initialize the SMS emulation */
    sms_init();

//SN76496_init(MASTER_CLOCK, 255, 44100); //VBT sample rate
//SN76496_init(MASTER_CLOCK, 255, 30720);
//SN76496_init(MASTER_CLOCK, 7680);
PSG_Init(MASTER_CLOCK, 7680);

//30720
}

