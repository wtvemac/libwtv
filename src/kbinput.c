#include "kbinput.h"
#include "libc.h"
#include "regs_internal.h"
#include "leds.h" // remove after relase
#include "display.h" // remove after relase
#include "storage/nvram.h" // remove after relase
#include "audio.h" // remove after relase
#include "graphics.h" // remove after relase
#include "wtvsys.h"

static uint32_t Q[4096], c = 362436;

uint32_t irHead;
uint32_t irTail;

rawIR irData[kIRBufSize];

uint32_t irSampleTailEMAC;
uint32_t irSampleHeadEMAC;
uint32_t irSamplesEMAC[kIRBufSize];
uint8_t sonyThing1;
uint8_t sonyThing2;
uint8_t sonyThing3;
uint8_t sonyThing4;
uint32_t sonyThing5[2];
uint8_t sonyThing6;
uint8_t sonyThing7;
uint8_t sejinThing1;
uint8_t sejinThing2;
uint32_t sejinThing3;
uint8_t sejinThing4;
uint8_t sejinThing5;
uint8_t sejinThing6;
uint8_t sejinThing7;


void init_rand(uint32_t x)
{
    int i;

    Q[0] = x;
    Q[1] = x + PHI;
    Q[2] = x + PHI + PHI;

    for (i = 3; i < 4096; i++)
            Q[i] = Q[i - 3] ^ Q[i - 2] ^ PHI ^ i;
}

uint32_t rand_cmwc()
{
    uint64_t t, a = 18782LL;
    static uint32_t i = 4095;
    uint32_t x, r = 0xfffffffe;
    i = (i + 1) & 4095;
    t = a * Q[i] + c;
    c = (t >> 32);
    x = t + c;
    if (x < c) {
            x++;
            c++;
    }
    return (Q[i] = r - x);
}

void keyboard_init()
{
	init_rand(2222);

	for(int iii=0;iii!=kIRBufSize;iii++)
	{
		irSamplesEMAC[iii] = 0;
	}
	irSampleHeadEMAC = irSampleTailEMAC = 0;
	sonyThing1 = 0;
	sonyThing2 = 0;
	sonyThing3 = 0;
	sonyThing4 = 0;
	//sonyThing5 = 0;
	sonyThing6 = 0;
	sejinThing1 = 1;
	sejinThing2 = 0;
	sejinThing3 = 0;
	sejinThing4 = 0;
	sejinThing5 = 0;
	sejinThing6 = 0;
	sejinThing7 = 0;

	REGISTER_WRITE(DEV_IR_IN_SAMPLE_TICKS, kIRSampleDelayEMAC * kIRSampleTicksEMAC);
	REGISTER_WRITE(DEV_IR_IN_REJECT_TIME, 0x0a);
	REGISTER_WRITE(DEV_IR_IN_STAT_CNTL, 0x3083);


	while(*((volatile uint32_t *)DEV_IR_IN_TRANS_DATA) != 0) { } // DrainIRInFIFOEMAC();
}


void ValidateSejinDataEMAC()
{
	rawIR newRawIR;

	//printf("ValidateSejinDataEMAC?\x0a\x0d");

	if(sejinThing6 != 0)
	{
		sejinThing5--;
	}

	if(sejinThing6 == 0 || !((sejinThing5 & 1) == 0 && sejinThing6 == 0))
	{
		uint32_t _irHead = (irHead + 1) & kIRBufMask;
		
		if(_irHead != irTail)
		{
			//printf("ValidateSejinDataEMAC!\x0a\x0d");

			newRawIR.kbData = sejinThing2;
			newRawIR.category1 = (sejinThing3 >> 0x17);
			newRawIR.category2 = 0;
			newRawIR.button = 0;
			newRawIR.time = 0;

			irData[_irHead] = newRawIR;
			irHead = _irHead & kIRBufMask;
		}
	}
}

void SejinGetBitEMAC(uint32_t irbit)
{

	sejinThing3 >>= 1;
	if(irbit == 1)
	{
		sejinThing3 |= 0x80000000;
		sejinThing5++;
	}

	sejinThing4++;

	if(sejinThing4 == 10)
	{
		sejinThing6 = irbit;
	}
}

void SejinTestBitTime_0EMAC(uint32_t irbit, uint32_t irtime, uint32_t a, uint32_t b)
{
	if(irbit == 0 && (a * kSejinBitOffMin) <= irtime && irtime <= (a * kSejinBitOffMax))
	{
		sejinThing1 = b;
	}
	else
	{
		sejinThing1 = 1;
	}
}

void SejinTestBitTime_1EMAC(uint32_t irbit, uint32_t irtime, uint32_t a, uint32_t b)
{
	if(irbit == 1 && (a * kSejinBitOnMin) <= irtime && irtime <= (a * kSejinBitOnMax))
	{
		sejinThing1 = b;
	}
	else
	{
		sejinThing1 = 1;
	}
}

void DecodeSejinEMAC(uint32_t irbit, uint32_t irtime)
{
	//printf("\t-->sejinThing1=%08x\x0a\x0d", sejinThing1);

	
	switch(sejinThing1)
	{
		// 0|0010,1 00||0
		
		case 1:
			sejinThing2 = 0;
			sejinThing3 = 0;
			sejinThing4 = 0;
			sejinThing5 = 0;
			sejinThing6 = 0;
			sejinThing7 = 0;

		case 2:
			SejinTestBitTime_0EMAC(irbit, irtime, 3, 3);
			break;

		case 3:
			SejinTestBitTime_1EMAC(irbit, irtime, 1, 4);
			break;

		case 4:
			SejinTestBitTime_0EMAC(irbit, irtime, 1, 5);
			break;

		case 5:
			SejinTestBitTime_1EMAC(irbit, irtime, 1, 6);
			break;

		case 6:
			SejinTestBitTime_0EMAC(irbit, irtime, 2, 7);
			if(sejinThing1 == 1)
			{
				SejinTestBitTime_0EMAC(irbit, irtime, 1, 10); // start break?
			}
			break;

		case 7:
			SejinTestBitTime_1EMAC(irbit, irtime, 1, 8);
			break;

		case 8:
			SejinTestBitTime_0EMAC(irbit, irtime, 1, 9);
			break;

		case 9:
			SejinTestBitTime_1EMAC(irbit, irtime, 1, 11);
			if(sejinThing1 != 1)
			{
				sejinThing2 = 0x94;
			}
			break;

		case 10:
			SejinTestBitTime_1EMAC(irbit, irtime, 4, 11);
			if(sejinThing1 != 1)
			{
				sejinThing2 = 0xd4;
			}
			break;
		case 11:
		case 12:
			if(irbit == 0)
			{
				uint32_t i = irtime - kSejinHalfBitcell;
				while(i >= kSejinBitcell && sejinThing4 < 9)
				{
					sejinThing4++;

					i -= kSejinBitcell;
				}

				if(sejinThing4 == 8)
				{
					SejinGetBitEMAC(1);
				}
				
				if(sejinThing4 == 9)
				{
					ValidateSejinDataEMAC();
					sejinThing1 = 1;
				}
				else
				{
					sejinThing1 = 13;
				}
			}
			else
			{
				sejinThing1 = 1;
			}
			break;

		case 13:
			SejinGetBitEMAC(irbit);

			uint32_t i = irtime - kSejinHalfBitcell;
			while(i >= kSejinBitcell && sejinThing4 < 9)
			{
				SejinGetBitEMAC(irbit);

				i -= kSejinBitcell;
			}

			if(sejinThing4 == 8)
			{
				SejinGetBitEMAC(1);
			}

			if(sejinThing4 == 9)
			{
				ValidateSejinDataEMAC();
				sejinThing1 = 1;
			}
			break;
	}



	//return sejinThing1;
}

void poopieit()
{
	printf("Draw the poopie!\x0a\x0d");
	//graphics_draw_poopie(display_get(), (rand_cmwc() % (560 - 64)), (174 + (rand_cmwc() % 168)));
	printf("Play poopie start...\x0a\x0d");
	play_poopie();
	printf("Play poopie end...\x0a\x0d");
}

void keyboard_poll()
{
	uint32_t irSampleHeadCopy = 0x00;
	uint32_t lastkey = 0x00;

	while(true)
	{
		//printf("keyboard_poll\x0a\x0d");
		while(true)
		{
			uint32_t irdata = *((volatile uint32_t *)DEV_IR_IN_TRANS_DATA);

			if((irdata & 0xf000) != 0)
			{
				uint32_t _irSampleHeadEMAC = (irSampleHeadEMAC + 1) & kIRBufMask;
				if(_irSampleHeadEMAC != irSampleTailEMAC)
				{
					irSampleHeadEMAC = _irSampleHeadEMAC;
					irSamplesEMAC[irSampleHeadEMAC] = irdata & 0xFFFF;
				}

				//printf("irdata: 0x%08x\x0a\x0d", irdata);
			}
			else
			{
				break;
			}
		}

		uint32_t irHeadCopy;

		irSampleHeadCopy = irSampleHeadEMAC;
		while(irSampleTailEMAC != irSampleHeadCopy)
		{
			if(irSampleTailEMAC != irSampleHeadEMAC)
			{
				irSampleTailEMAC = (irSampleTailEMAC + 1) & kIRBufMask;

				if((irSamplesEMAC[irSampleTailEMAC] & 0xf000) != 0xf000)
				{
					uint32_t irbit = (irSamplesEMAC[irSampleTailEMAC] & 0x0fff) >> 0x0b;
					uint32_t irtime = (irSamplesEMAC[irSampleTailEMAC] & 0x07ff) * kIRSampleDelayEMAC; /* gUsPerTick = 0x0f */

					DecodeSejinEMAC(irbit, irtime); // Keyboard

					irHeadCopy = irHead & kIRBufMask;

					//printf("\tirbit=0x%08x, irtime=0x%08x, irHeadCopy=%08x, irTail=%08x\x0a\x0d", irbit, irtime, irHeadCopy, irTail);


					while ((irTail & kIRBufMask) != (irHeadCopy & kIRBufMask))
					{
						irTail = (irTail + 1) & kIRBufMask;

						if(((irData[irTail].kbData & kSejinIDMask) == kSejinKeyboardID) &&	(irData[irTail].category1 < kIRKeyMapTableSize) && irData[irTail].category1)
						{
							printf("IR KEYPRESS: index=%02x, data=%08x\x0a\x0d", irData[irTail].category1, irData[irTail].kbData);

							/*if(irData[irTail].category1 == 0x38) // Space bar
							{
								*((volatile uint32_t *)VID_NSTART) += 0x01;
							}
							else if(irData[irTail].category1 == 0x20) // Caps lock
							{
								*((volatile uint32_t *)VID_NSTART) -= 0x01;
							}

							else if(irData[irTail].category1 == 0x09) // Caps lock
							{
								*((volatile uint32_t *)POT_HSTART) += 0x01;
							}
							else if(irData[irTail].category1 == 0x1e) // A
							{
								*((volatile uint32_t *)POT_HSTART) -= 0x01;
							}

							else if(irData[irTail].category1 == 0x04) // ~
							{
								*((volatile uint32_t *)POT_VSTART) += 0x01;
							}
							else if(irData[irTail].category1 == 0x1d) // ~
							{
								*((volatile uint32_t *)POT_VSTART) -= 0x01;
							}*/


							if(irData[irTail].category1 == 0x1b) // power key (reset to bootrom)
							{
								if(lastkey != irData[irTail].category1)
								{
									printf("Power cycle box via warm reset command...\x0a\x0d");

									printf("Turning all LEDs on.\x0a\x0d");
									set_leds(LEDS_ALL);

									printf("Turning the screen off.\x0a\x0d");
									display_disable();

									printf("Setting MEM_CMD to preserve memory and BUS_RESET_CAUSE_SET to reset. This typically only works on a Derby or Solo rev >= 1.3. You may need to unplug the box if this gets stuck...\x0a\x0d");
									*((volatile uint32_t *)MEM_CMD) = 0xd0000000;
									*((volatile uint32_t *)BUS_RESET_CAUSE_SET) = 0x00000004;

									while(1) { *((volatile uint32_t *)BUS_CHIP_CNTL); }
								}
							}
							else if(irData[irTail].category1 == 0x0c) // F1 key (reset box)
							{
								if(lastkey != irData[irTail].category1)
								{
									printf("Boot to bootrom hit...\x0a\x0d");

									printf("Turning all LEDs on.\x0a\x0d");
									set_leds(LEDS_ALL);

									printf("Turning the screen off.\x0a\x0d");
									display_disable();

									printf("Writing to IIC NVRAM (secondary NVRAM) to indicate a skip to the bootrom.\x0a\x0d");
									set_box_flag(BOX_FLAG_BOOTROM_BOOT, true);
									
									printf("Setting BUS_RESET_CAUSE_SET to reset.\x0a\x0d");
									//*((volatile uint32_t *)MEM_CMD) = 0xd0000000;
									*((volatile uint32_t *)BUS_RESET_CAUSE_SET) = 0x00000004;

									*((volatile uint32_t *)0xA0000404) = 0x0000000b;
									*((volatile uint32_t *)0xA00007C4) = 0x00000000;
									
									

									//printf("Breaking out into the bootrom.\x0a\x0d");
									//int (*func)(void) = (int (*)(void))0x80200000;
									//func();
								}
								
							}
							else if(irData[irTail].category1 == 0x14) // F2 key (turn screen off)
							{
								if(lastkey != irData[irTail].category1)
								{
									printf("Power cycle box via watchdog hit...\x0a\x0d");
									
									printf("Turning all LEDs on.\x0a\x0d");
									set_leds(LEDS_ALL);
									
									printf("Turning the screen off.\x0a\x0d");
									display_disable();

									printf("Enabling watchdog timer.\x0a\x0d");
									uint32_t chipval = *((volatile uint32_t *)BUS_CHIP_CNTL) & 0x3fffffff;
									*((volatile uint32_t *)BUS_CHIP_CNTL) = chipval | 0x00000000;
									*((volatile uint32_t *)BUS_CHIP_CNTL) = chipval | 0x40000000;
									*((volatile uint32_t *)BUS_CHIP_CNTL) = chipval | 0x80000000;
									*((volatile uint32_t *)BUS_CHIP_CNTL) = chipval | 0xc0000000;
									
									printf("Idling around like an idiot. The watchdog is going to get us soon!\x0a\x0d");
									while(1) { *((volatile uint32_t *)BUS_CHIP_CNTL); }
								}
							}
							else if(irData[irTail].category1 == 0x11) // F3 key (turn screen on)
							{
								if(lastkey != irData[irTail].category1)
								{
									printf("Disable screen hit...\x0a\x0d");

									printf("Turning all LEDs on.\x0a\x0d");
									set_leds(LEDS_ALL);

									printf("Turning the screen off.\x0a\x0d");
									display_disable();
								}
							}
							else if(irData[irTail].category1 == 0x13) // F4 key (turn screen on)
							{
								if(lastkey != irData[irTail].category1)
								{
									printf("Enable screen hit...\x0a\x0d");

									printf("Turning only the power LED on.\x0a\x0d");
									set_leds(LED_POWER);

									printf("Turning the screen on.\x0a\x0d");
									display_enable();
								}
							}
							else if(irData[irTail].category1 == 0x2b) // Ctrl 
							{
								if(lastkey != irData[irTail].category1)
								{
								}
							}
							else if(irData[irTail].kbData == 0x00000094)
							{
								poopieit();
							}

							lastkey = irData[irTail].category1;
						}
					}
				}
			}
		}
	}
}
