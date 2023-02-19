#include "../shared.h"
#define SOUND 1
#define CED 1
/*
#ifdef CED
#include "snd/empty_drv.h"
#include "snd/sn76496ced.h"
#endif*/

extern t_sms sms;
extern t_cart cart;
extern unsigned int sound;
extern unsigned dummy_write[0x100];

static __inline__ void cpu_writemem8(unsigned int address, unsigned int data)
{
	sms.ram[address & 0x1FFF] = data;
// data & cart.pages, and set cart.pages to one less than you are
        uint32 offset = (data % cart.pages) << 14; // VBT à corriger
// vbt 15/05/2008 : exophase :	 data & cart.pages, and set cart.pages to one less than you are
		sms.fcr[address& 3] = data;

        switch(address & 3)
        {
			case 0:

                if(data & 8)
                {
					offset = (data & 0x4) ? 0x4000 : 0x0000;

					z80_map_fetch(0x8000, 0xBFFF, (unsigned)(sms.sram + offset));
					z80_map_read(0x8000, 0xBFFF, (unsigned)(sms.sram + offset));
					z80_map_write(0x8000, 0xBFFF, (unsigned)(sms.sram + offset));
                }
                else
                {
					offset = ((sms.fcr[3] % cart.pages) << 14);
// vbt 15/05/2008 : exophase :	 data & cart.pages, and set cart.pages to one less than you are
					z80_map_fetch(0x8000, 0xBFFF, (unsigned)(cart.rom + offset));
					z80_map_read(0x8000, 0xBFFF, (unsigned)(cart.rom + offset));
					z80_map_write(0x8000, 0xBFFF, (unsigned)(dummy_write));
                }
				break;
            case 1:
				z80_map_fetch(0x0000, 0x3FFF, (unsigned)(cart.rom + offset));
				z80_map_read(0x0000, 0x3FFF, (unsigned)(cart.rom + offset));
			break;

            case 2:
				z80_map_fetch(0x4000, 0x7FFF, (unsigned)(cart.rom + offset));
				z80_map_read(0x4000, 0x7FFF, (unsigned)(cart.rom + offset));
            break;

            case 3:

			if(!(sms.fcr[0] & 0x08))
            {
					z80_map_fetch(0x8000, 0xBFFF, (unsigned)(cart.rom + offset));
					z80_map_read(0x8000, 0xBFFF, (unsigned)(cart.rom + offset));
            }
            break;
        }
    return;
}
//---------------------------------------------------------------------------
static void cz80_z80_writeport16(unsigned short PortNo, unsigned char data)
{
    switch(PortNo & 0xFF)
    {
        case 0x01: /* GG SIO */
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
        case 0x06: /* GG STEREO */
        case 0x7E: /* SN76489 PSG */
        case 0x7F:
#ifdef SOUND
			if (sound)
#ifndef CED	
//				SN76496Write(0,data);
				PSG_Write(data);
#else
				sn76496_w(data);
#endif

#endif
           break;

        case 0xBE: /* VDP DATA */
            vdp_data_w(data);
            break;

        case 0xBD: /* VDP CTRL */ 
        case 0xBF:
//			printf ("write vdp_ctrl_w");
            vdp_ctrl_w(data);
            break;

        case 0xF0: /* YM2413 */
        case 0xF1:
//            ym2413_write(0, PortNo & 1, data);
            break;

        case 0xF2: /* YM2413 DETECT */
//            sms.port_F2 = (data & 1);
            break;



        case 0x3F: /* TERRITORY CTRL. */
             sms.port_3F = ((data & 0x80) | (data & 0x20) << 1) & 0xC0;
            if(sms.country == TYPE_DOMESTIC) sms.port_3F ^= 0xC0;
            break;
    }
}
//---------------------------------------------------------------------------
static unsigned char cz80_z80_readport16(unsigned short PortNo)
{
    switch(PortNo & 0xFF)
    {
        case 0x7E: /* V COUNTER */
            return (vdp_vcounter_r());
            break;
    
        case 0x7F: /* H COUNTER */
		//	printf("vdp_hcounter_r\n");
            return (vdp_hcounter_r());
            break;
    
        case 0x00: /* INPUT #2 */
			return 0xff;
//            temp = 0xFF;
//			return (update_system());
//            if(input.system & INPUT_START) temp &= ~0x80;
//            if(sms.country == TYPE_DOMESTIC) temp &= ~0x40;
//            return (temp);
    
		case 0xC0: /* INPUT #0 */  
		case 0xDC:
			return (update_input());
//			return 0xff;
		case 0xC1: /* INPUT #1 */
		case 0xDD:
//			return 0xff;
			return update_input2();

        case 0xBE: /* VDP DATA */
//			printf ("read port vdp_data_r\n");
            return (vdp_data_r());
    
        case 0xBD:
        case 0xBF: /* VDP CTRL */
//			printf ("read port vdp_ctrl_r\n");
            return (vdp_ctrl_r());

        case 0xF2: /* YM2413 DETECT */
//            return (sms.port_F2);
            break;
    }
//    return (0xFF);      
    return (0);      
}

void z80_init(void)
{
	z80_init_memmap();
//	z80_map_fetch(0x0000, 0xBFFF, (unsigned char *)&cart.rom);
//	z80_map_fetch(0xC000, 0xFFFF, (unsigned char *)&sms.ram);
#define Z80_MAP_HANDLED 1
#define Z80_MAP_DIRECT  0  /* Reads/writes are done directly */
//z80_add_read(0x0000, 0xFFFF, Z80_MAP_HANDLED, &cpu_readmem8); 

/* Bank #0 */ 
z80_map_fetch (0x0000, 0x3FFF, (unsigned char *)(&cart.rom[0])); 
z80_map_read (0x0000, 0x3FFF, (unsigned char *)(&cart.rom[0])); 

/* Bank #1 */ 
z80_map_fetch (0x4000, 0x7FFF, (unsigned char *)(&cart.rom[0x4000])); 
z80_map_read (0x4000, 0x7FFF, (unsigned char *)(&cart.rom[0x4000])); 

/* Bank #2 */ 
z80_map_fetch (0x8000, 0xBFFF, (unsigned char *)(&cart.rom[0x8000])); 
z80_map_read (0x8000, 0xBFFF, (unsigned char *)(&cart.rom[0x8000])); 

/* RAM */ 
z80_map_fetch (0xC000, 0xDFFF, (unsigned char *)(&sms.ram[0])); 
z80_map_read (0xC000, 0xDFFF, (unsigned char *)(&sms.ram[0])); 
z80_map_write (0xC000, 0xDFFF, (unsigned char *)(&sms.ram[0])); 

/* RAM (mirror) */ 
z80_map_fetch (0xE000, 0xFFFF, (unsigned char *)(&sms.ram[0])); 
z80_map_read (0xE000, 0xFFFF, (unsigned char *)(&sms.ram[0])); 
z80_map_write (0xE000, 0xFFFF, (unsigned char *)(&sms.ram[0])); 

//z80_add_write(0xC000, 0xFFFF, Z80_MAP_DIRECT, (void *)(&sms.ram[0]));
//z80_add_write(0xFFFC, 0xFFFF, Z80_MAP_HANDLED, (void *)&cpu_writemem8);
z80_add_write(0x0000, 0xFFFF, Z80_MAP_HANDLED, (void *)&cpu_writemem8);

	z80_end_memmap();     

	z80_set_in((unsigned char (*)(short unsigned int))&cz80_z80_readport16);
	z80_set_out((void (*)(short unsigned int, unsigned char))&cz80_z80_writeport16);
//	z80_set_fetch_callback(&debug);

	z80_reset();
}


void sms_reset(void)
{
	z80_reset();
    /* Clear SMS context */
    memset(dummy_write, 0, 0x100);
    memset(sms.ram, 0, 0x2000);
    memset(sms.sram, 0, 0x8000);
    sms.port_3F = sms.port_F2 = sms.irq = 0x00;
//    sms.psg_mask = 0xFF;

    /* Load memory maps with default values */
	z80_map_read (0x0000, 0x3FFF, &cart.rom[0]); 

	z80_map_read (0x4000, 0x7FFF, &cart.rom[0x4000]); 
	z80_map_read (0x8000, 0xBFFF, &cart.rom[0x8000]); 
	z80_map_write(0x0000, 0xBFFF, (unsigned)dummy_write);

	z80_map_read (0xC000, 0xDFFF, (unsigned char *)(&sms.ram[0])); 
	z80_map_write (0xC000, 0xDFFF, (unsigned char *)(&sms.ram[0])); 

	z80_map_read (0xE000, 0xFFFF, (unsigned char *)(&sms.ram[0])); 
	z80_map_write (0xE000, 0xFFFF, (unsigned char *)(&sms.ram[0])); 

#define Z80_MAP_HANDLED 1
//	z80_add_write(0xFFFC, 0xFFFF, Z80_MAP_HANDLED, (void *)&cpu_writemem8);
//	z80_add_read(0x0000, 0xFFFF, Z80_MAP_HANDLED, (void *)&cpu_readmem8);
	z80_add_write(0xFFFC, 0xFFFF, Z80_MAP_HANDLED, (void *)&cpu_writemem8);

	sms.fcr[0] = 0x00;
    sms.fcr[1] = 0x00;
    sms.fcr[2] = 0x01;
    sms.fcr[3] = 0x00;
}
