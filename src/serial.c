#include "serial.h"
#include "regs_internal.h"
#include "wtvsys.h"
#include "leds.h"
#include <string.h>

//serial_context init_suc_serial(char port, unsigned long baudRate, char parityBits, char stopBits)
void serial_io_init()
{
	if(is_solo_box())
	{
		unsigned long base = SUC_SET0_BASE;

		REGISTER_WRITE((base + SUC_MCD), 0x0);
		REGISTER_WRITE((base + SUC_SCD1), 0x0);
		REGISTER_WRITE((base + SUC_LSCR), 0x0);
		REGISTER_WRITE((base + SUC_SPIO_CNTL), 0x0);

		REGISTER_WRITE((base + SUC_TSRCR), SUC_TSR_EN);
		REGISTER_WRITE((base + SUC_RSRCR), SUC_RSR_EN);

		REGISTER_WRITE((base + SUC_TFFCR), SUC_FF_ENABLE);
		REGISTER_WRITE((base + SUC_RFFCR), SUC_FF_ENABLE);
		REGISTER_WRITE((base + SUC_LSTPBITS), 0x1 + 1);
		REGISTER_WRITE((base + SUC_SPIO_EN_SET), SUC_SPIO_SCCTXD_EN);
		REGISTER_WRITE((base + SUC_GPIO_DIR_STATE), SUC_GPIO_DIR_SCCTXD);

		REGISTER_WRITE((base + SUC_CCR), SUC_SAMPCLK_EN | SUC_MASTCLK_EN);
		REGISTER_WRITE((base + SUC_LCR), SUC_8BIT_CHAR);

		REGISTER_WRITE((base + SUC_GPIO_VAL_STATE), SUC_GPIO_VAL_SCRRTS_N);
		REGISTER_WRITE((base + SUC_GPIO_DIR_SET), SUC_GPIO_DIR_SCRRTS_N);

		REGISTER_WRITE((base + SUC_SCDO), 0x2c); // baud rate

		REGISTER_WRITE((base + SUC_GPIO_DIR_CLEAR), SUC_GPIO_DIR_DCD_N | SUC_GPIO_DIR_SCICTS_N);

		REGISTER_WRITE((base + SUC_IOOD), SUC_IOOD_DCD_N | SUC_IOOD_SCICTS_N | SUC_IOOD_SCRRTS_N | SUC_IOOD_SCPTR_N | SUC_IOOD_SCCTXD | SUC_IOOD_SCDRXD);
	}
}

//void wait_tx_ready(serial_context port)
void serial_wait_tx_ready()
{
	unsigned long base = SUC_SET0_BASE;

	while(REGISTER_READ((base + SUC_TFFCNT)) >= REGISTER_READ((base + SUC_TFFMAX)));
}


void serial_put_string(char const *msg)
{
	for(int i = 0; i < strlen(msg); i++)
	{
		serial_put_char(msg[i]);
	}
}

void serial_put_char(char chr)
{
	if(is_spot_box())
	{
		REGISTER_WRITE(DEV_SMARTCARD_CNTL, 0x00);
		REGISTER_WRITE(DEV_SMARTCARD_CNTL, 0x01);

		for(int i = 0; i < 8; i++)
		{
			REGISTER_WRITE(DEV_SMARTCARD_CNTL, !(chr & 1));

			chr >>= 1;
		}

		REGISTER_WRITE(DEV_SMARTCARD_CNTL, 0x00);
	}
	else
	{
		unsigned long base = SUC_SET0_BASE;

		serial_wait_tx_ready();

		REGISTER_WRITE((base + SUC_TFFHR), chr );
	}
}
