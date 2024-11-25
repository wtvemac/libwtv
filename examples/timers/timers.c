#include <stdint.h>
#include <libwtv.h>

static volatile uint32_t t1 = 0; // increment every ms
static volatile uint32_t t2 = 0; // increment every .5 sec
static volatile uint32_t t3 = 0; // increment every 1 sec

void one_msec(int ovfl)
{
    t1 += 1;
}

void half_sec(int ovfl)
{
    t2 += 1;
}

void one_sec(int ovfl)
{
    t3 += 1;
}

extern void __init_interrupts(void);


#define	kSejinBitOffMin		0x2cb
#define	kSejinBitOffMax		0x3e3
#define	kSejinBitOnMin		0x22b
#define	kSejinBitOnMax		0x32a
int main(void)
{
    heap_init2();
	__init_interrupts();

    //set_serial_output(true);

    //set_leds(1);

    //printf("ps2_init=%02x\n", ps2_init());printf("ps2_get_identity=%s\n", ps2_get_identity());
    ir_init();

    //display_init(CONSOLE_RESOLUTION, FMT_YUV16, 2, 0x0);

    //console_init();
    //console_alloc(10, 20, 548, 132, 2, 2, 4, 2, false);
    //console_set_render_mode(RENDER_MANUAL);

	//printf("COOL BEANS v1.0!\x0a\x0d");
	//printf("Built using eMac's WebTV SDK...\x0a\x0d");
	//printf("\x0a\x0d");

    //ssid_t ssid = get_ssid();

	//printf("CHIP ID:      0x%08x\x0a\x0d", get_chip_id());
	//printf("SYSCONFIG:    0x%08x\x0a\x0d", get_sysconfig());
	//printf("RAM SIZE:     %d MB\x0a\x0d", (get_memory_size() / 1024 / 1024));
	//printf("SSID:         %s\x0a\x0d", get_ssid_string());
	//printf("HD LOCK 0x36: %s\x0a\x0d", get_atapwd_random_string());

    //console_render();

	//printf("get_ata_userpwd: %s\x0a\x0d", get_ata_pwd_string(get_ata_userpwd()));
	//printf("get_ata_masterpwd: %s\x0a\x0d", get_ata_pwd_string(get_ata_masterpwd()));

    bool get_scancode1 = false;
    bool get_scancode2 = false;
    uint8_t lastbit = 1;
    uint8_t bitloc = 0;
    uint8_t idbyte = 0x00;
    uint16_t coolers = 0x00;
    uint32_t scancode = 0x00;
    uint8_t walkback = 0x00;
    uint8_t wtf = 0x00;

    uint8_t cool = 0x00;
    uint32_t tottime = 0x00;
    uint16_t min0 = 0xffff;
    uint16_t max0 = 0x0000;
    uint16_t min1 = 0xffff;
    uint16_t max1 = 0x0000;

    uint32_t data = 0x00000000;
    uint8_t totalbits = 0x00;
    uint8_t scancode2 = 0x00;
    uint8_t makebreak = 0x00;

    serial_put_string("Loop2\x0a\x0d");

    while (1)
    {
        uint32_t ir_data = dequeue_ir_buffer();

        if(ir_data != 0xffff)
        {
            printf("\tir_data=0x%08x\x0a\x0d", ir_data);
        }

        continue;

        typedef struct ir_bit_time_limit_s
        {
            uint16_t max;
            uint16_t med;
            uint16_t min;
        } ir_bit_time_limit_t;

        // (1000 * 1000) / 38KHz = 26.3us * 30 pulses per bit = 789us per bit / 15us per sample = 52.6 samples
        ir_bit_time_limit_t ir_bit_time_limits[] = {
            // 0
            {
                .max = 0x3d,
                .med = 0x30,
                .min = 0x2a
            },
            // 1
            {
                .max = 0x38,
                .med = 0x2b,
                .min = 0x25
            },
        };
        #define IR_TRANSITION_SAMPLE_COUNT(ir_data)     ((ir_data >> 0x0c))
        #define IR_TRANSITION_CURRENT_VALUE(ir_data)    ((ir_data >> 0x0b) & 0x01)
        #define IR_TRANSITION_CURRENT_TIME(ir_data)     ((ir_data & 0x07ff))
        #define IS_IR_TRANSITION_GOOD(ir_data)          (IR_TRANSITION_SAMPLE_COUNT(ir_data) != 0xf && (ir_bit_time_limits[IR_TRANSITION_CURRENT_VALUE(ir_data) & 0x1].min <= IR_TRANSITION_CURRENT_TIME(ir_data) && (IR_TRANSITION_CURRENT_TIME(ir_data) <= ir_bit_time_limits[IR_TRANSITION_CURRENT_VALUE(ir_data) & 0x1].max || IR_TRANSITION_CURRENT_TIME(ir_data) >= (ir_bit_time_limits[IR_TRANSITION_CURRENT_VALUE(ir_data) & 0x1].min * 2))))
        if(IS_IR_TRANSITION_GOOD(ir_data))
        {
            uint16_t time2 = IR_TRANSITION_CURRENT_TIME(ir_data);
            uint8_t bit2 = IR_TRANSITION_CURRENT_VALUE(ir_data);

            uint8_t bitcount = 1;
            if(time2 > ir_bit_time_limits[bit2].max)
            {
                bitcount = (time2 / ir_bit_time_limits[bit2].med);
            }

            if(time2 == 0x7ff)
            {
                data = 0x00;
                makebreak = 0x00;
                scancode2 = 0x00;
                totalbits = 0x00;
            }
            else
            {
                for(uint8_t i = 0; i < bitcount; i++)
                {
                    if(totalbits < 22)
                    {
                        data |= (bit2 << totalbits);
                        totalbits++;
                    }
                    else
                    {
                        printf("\tKilled\x0a\x0d");
                        break;
                    }
                }
            }

            if(((ir_data >> 0x0b) & 0x1) == cool)
            {
                //printf("\x0a\x0d");
                tottime = 0x00;
            }
            else
            {
                // 0=0x35[0x31b]-0x3b[0x375], 1=0x2a[0x276]-0x31[0x2df]
                if(((ir_data >> 0x0c) & 0xf) > 0 && time2 > 0x22 && time2 < 0x49)
                {
                    if(((ir_data >> 0x0b) & 0x1) == 1)
                    {
                        if(time2 < min1)
                        {
                            min1 = time2;
                        }
                        if(time2 > max1)
                        {
                            max1 = time2;
                        }
                    }
                    else
                    {
                        if(time2 < min0)
                        {
                            min0 = time2;
                        }
                        if(time2 > max0)
                        {
                            max0 = time2;
                        }
                    }
                }
            }

            tottime += (ir_data & 0x07ff);

            printf(
                "0x%04x: time=0x%03x, val=0x%02x, bitcount=0x%02x, data=%c|%c,%c,%c%c%c%c%c%c%c|%c_%c|%c,%c[%c]%c%c,%c%c%c%c|%c, totalbits=%02x, scancode2=%02x\x0a\x0d", 
                (ir_data), 
                (ir_data & 0x07ff), 
                ((ir_data >> 0x0b) & 0x1), 
                bitcount, 

                ((totalbits == 21) ? '*' : ((data) & 0x200000 ? '1' : '0')),
                ((totalbits == 20) ? '*' : ((data) & 0x100000 ? '1' : '0')),
                ((totalbits == 19) ? '*' : ((data) & 0x080000 ? '1' : '0')),
                ((totalbits == 18) ? '*' : ((data) & 0x040000 ? '1' : '0')),
                ((totalbits == 17) ? '*' : ((data) & 0x020000 ? '1' : '0')),
                ((totalbits == 16) ? '*' : ((data) & 0x010000 ? '1' : '0')),
                ((totalbits == 15) ? '*' : ((data) & 0x008000 ? '1' : '0')),
                ((totalbits == 14) ? '*' : ((data) & 0x004000 ? '1' : '0')),
                ((totalbits == 13) ? '*' : ((data) & 0x002000 ? '1' : '0')),
                ((totalbits == 12) ? '*' : ((data) & 0x001000 ? '1' : '0')),
                ((totalbits == 11) ? '*' : ((data) & 0x000800 ? '1' : '0')),
                ((totalbits == 10) ? '*' : ((data) & 0x000400 ? '1' : '0')),
                ((totalbits == 9) ? '*' : ((data) & 0x000200 ? '1' : '0')),
                ((totalbits == 8) ? '*' : ((data) & 0x000100 ? '1' : '0')),
                ((totalbits == 7) ? '*' : ((data) & 0x000080 ? '1' : '0')),
                ((totalbits == 6) ? '*' : ((data) & 0x000040 ? '1' : '0')),
                ((totalbits == 5) ? '*' : ((data) & 0x000020 ? '1' : '0')),
                ((totalbits == 4) ? '*' : ((data) & 0x000010 ? '1' : '0')),
                ((totalbits == 3) ? '*' : ((data) & 0x000008 ? '1' : '0')),
                ((totalbits == 2) ? '*' : ((data) & 0x000004 ? '1' : '0')),
                ((totalbits == 1) ? '*' : ((data) & 0x000002 ? '1' : '0')),
                ((totalbits == 0) ? '*' : ((data) & 0x000001 ? '1' : '0')),

                totalbits, 
                ((data >> 0x0c) & 0x7f)
            );

            if(totalbits >= 22 || time2 == 0x7ff)
            {
                //printf("\x0a\x0d");
                data = 0x00;
                makebreak = 0x00;
                scancode2 = 0x00;
                totalbits = 0x00;
            }

            cool = ((ir_data >> 0x0b) & 0x1);

            continue;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            uint8_t bit =  !!(ir_data & 0x8000);
            uint16_t time = ((ir_data & 0x07ff) * 0x0f);
            uint16_t timemod = 1;
            uint16_t timemin = kSejinBitOffMin;
            uint16_t timemax = kSejinBitOffMax;
            if(bit == 1)
            {
                timemin = kSejinBitOnMin;
                timemax = kSejinBitOnMax;
            }
            else
            {
                timemin = kSejinBitOffMin;
                timemax = kSejinBitOffMax;
            }

            bitloc++;

            if(get_scancode2)
            {
                ///
                scancode >>= 1;
                if(bit == 1)
                {
                    scancode |= 0x80000000;
                    walkback++;
                }
                coolers++;
                if(coolers == 10)
                {
                    wtf = bit;
                }
                ///

                uint32_t i = time - 0x154;
                while(i >= 0x303 && coolers < 9)
                {
                    ///
                    scancode >>= 1;
                    if(bit == 1)
                    {
                        scancode |= 0x80000000;
                        walkback++;
                    }
                    coolers++;
                    if(coolers == 10)
                    {
                        wtf = bit;
                    }
                    ///

                    i -= 0x303;
                }

                if(coolers == 8)
                {
                    ///
                    scancode >>= 1;
                    scancode |= 0x80000000;
                    walkback++;
                    coolers++;
                    wtf = (coolers == 10);
                    ///
                }

                if(coolers == 9)
                {
                    if(wtf != 0)
                    {
                        walkback--;
                    }

                    if(wtf == 0 || !((walkback & 1) == 0 && wtf == 1))
                    {
                        printf("\t==================2> DATA=0x%02x, CAT=0x%08x\x0a\x0d\x0a\x0d", idbyte, (scancode >> 0x17));
                    }
                    get_scancode1 = false;
                    get_scancode2 = false;
                    lastbit = 1;
                    bitloc = 0;
                    idbyte = 0x00;
                    coolers = 0x00;
                    scancode = 0x00;
                    walkback = 0x00;
                    wtf = 0x00;
                }
            }
            else if(get_scancode1)
            {
                if(bit == 0)
                {
                    uint32_t i = time - 0x154;
                    while(i >= 0x303 && coolers < 9)
                    {
                        coolers++;
                        i -= 0x303;
                    }

                    if(coolers == 8)
                    {
                        scancode >>= 1;
                        scancode |= 0x80000000;
                        walkback++;
                        coolers++;
                        wtf = (coolers == 10);
                    }
                    
                    if(coolers == 9)
                    {
                        if(wtf != 0)
                        {
                            walkback--;
                        }

                        if(wtf == 0 || !((walkback & 1) == 0 && wtf == 1))
                        {
                            printf("\t==================1> DATA=0x%02x, CAT=0x%08x\x0a\x0d\x0a\x0d", idbyte, (scancode >> 0x17));
                        }
                        get_scancode1 = false;
                        get_scancode2 = false;
                        lastbit = 1;
                        bitloc = 0;
                        idbyte = 0x00;
                        coolers = 0x00;
                        scancode = 0x00;
                        walkback = 0x00;
                        wtf = 0x00;
                    }
                    else
                    {
                        get_scancode2 = true;
                    }
                }
                else
                {
                    printf("\tRESET2 [idbyte=0x%02x, bitloc=0x%02x, time=0x%02x, timemin=0x%02x, timemax=0x%02x]\x0a\x0d\x0a\x0d", idbyte, bitloc, time, (timemin * timemod), (timemax * timemod));
                    get_scancode1 = false;
                    get_scancode2 = false;
                    lastbit = 1;
                    bitloc = 0;
                    idbyte = 0x00;
                    coolers = 0x00;
                    scancode = 0x00;
                    walkback = 0x00;
                    wtf = 0x00;
                }
            }
            else
            {
                if(bitloc == 1)
                {
                    timemod = 3;
                }
                else if(bitloc == 2 || bitloc == 3 || bitloc == 4 || bitloc == 6 || bitloc == 7)
                {
                    timemod = 1;
                }
                else if(bitloc == 5)
                {
                    timemod = 2;
                }
                //
                else if(bitloc == 8)
                {
                    timemod = 2;
                }
                else if(bitloc == 9)
                {
                    timemod = 4;
                }

                //printf("\tbitloc=%02x, time=%02x, timemin=%02x, timemax=%02x\x0a\x0d", bitloc, time, (timemin * timemod), (timemax * timemod));

                //if(time >= (timemin) && time <= (timemax * timemod))
                if(time >= (timemin * timemod) && time <= (timemax * timemod))
                {
                    if(bitloc == 8)
                    {
                        get_scancode1 = true;
                        get_scancode2 = false;
                        idbyte = 0x94;
                        printf("\tidbyte=0x%02x\x0a\x0d", idbyte);
                    }
                    else if(bitloc == 9)
                    {
                        get_scancode1 = true;
                        get_scancode2 = false;
                        idbyte = 0xd4;
                        printf("\tidbyte=0x%02x\x0a\x0d", idbyte);
                    }

                }
                else if(bitloc == 5)
                {
                    timemod = 1;
                    if((time * timemod) >= timemin && (time * timemod) <= timemax)
                    {
                        bitloc = 8;
                    }
                }
                else
                {
                    printf("\tRESET1 [idbyte=0x%02x, bitloc=0x%02x, time=0x%02x, timemin=0x%02x, timemax=0x%02x]\x0a\x0d\x0a\x0d", idbyte, bitloc, time, (timemin * timemod), (timemax * timemod));
                    get_scancode1 = false;
                    get_scancode2 = false;
                    lastbit = 1;
                    bitloc = 0;
                    idbyte = 0x00;
                    coolers = 0x00;
                    scancode = 0x00;
                    walkback = 0x00;
                    wtf = 0x00;
                }
            }

            /*if(code < 0)
            {
                printf("code=%c, -0x%02x\n", (code * -1), (code * -1));
            }
            else
            {
                printf("code=%c, 0x%02x\n", code, code);
            }*/
        }
    }

    //timer_link_t *one_shot_t;
    //long long start, end;

    /*timer_init();
    start = timer_ticks();
    new_timer(TIMER_TICKS(1000), TF_CONTINUOUS, one_msec);
    new_timer(TIMER_TICKS(500000), TF_CONTINUOUS, half_sec);
    new_timer(TIMER_TICKS(1000000), TF_CONTINUOUS, one_sec);*/

    /* Main loop test */
    set_serial_output(false);
    while(1)
    {
        
        t3++;

        //printf("Running on: %s\n", is_spot_box() ? "SPOT" : "SOLO");
        printf("\tEvery msec    : %d\n", t1);
        printf("\tEvery half sec: %d\n", t2);
        printf("\tEvery sec     : %d\n", t3);


        /*if(t3 == 1000)
        {
            console_disable();
        }
        else if(t3 == 3000)
        {
            console_enable();
            break;
        }*/

        console_render();
    }

    while (1) ;
}
