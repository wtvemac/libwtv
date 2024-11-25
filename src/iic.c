#include <stdbool.h>
#include "iic.h"
#include "regs_internal.h"
#include "wtvsys.h"

uint8_t __iic_get_sda()
{
	
	return (REGISTER_READ(DEV_IIC_CNTL) & 1);
}

void __iic_set_sda(uint8_t bit)
{
	if(bit)
	{
		REGISTER_WRITE(DEV_IIC_CNTL, (REGISTER_READ(DEV_IIC_CNTL) & 0xfffffff9) | 0x00000002);
	}
	else
	{
		REGISTER_WRITE(DEV_IIC_CNTL, (REGISTER_READ(DEV_IIC_CNTL) & 0xfffffff9) | 0x00000004);
	}
}

uint8_t __iic_get_scl()
{
	return ((REGISTER_READ(DEV_IIC_CNTL) & 0x20) != 0);
}

void __iic_set_scl(uint8_t bit)
{
	if(bit)
	{
		REGISTER_WRITE(DEV_IIC_CNTL, (REGISTER_READ(DEV_IIC_CNTL) & 0xffffffe7) | 0x00000008);

		int wait_time = 0;
		while(!__iic_get_scl() && wait_time < 100)
		{
			wait_us(1);
			wait_time++;
		}
	}
	else
	{
		REGISTER_WRITE(DEV_IIC_CNTL, (REGISTER_READ(DEV_IIC_CNTL) & 0xffffffe7) | 0x00000010);
	}
}

void __iic_start()
{
	__iic_set_sda(1);
	__iic_set_scl(1);
	wait_us(5);

	__iic_set_sda(0);
	wait_us(4);

	__iic_set_scl(0);
	wait_us(5);
}

void __iic_send_bit(uint8_t bit)
{
	__iic_set_sda(bit);
	wait_us(1);
	
	__iic_set_scl(1);
	wait_us(4);
	
	__iic_set_scl(0);
	wait_us(5);

	wait_us(1);
}

uint8_t __iic_read_byte(uint32_t coolman)
{
	__iic_set_sda(1);

	uint8_t byte = 0x00;

	for(int i = 7; i >= 0; i--)
	{
		__iic_set_scl(1);
		wait_us(4);

		byte <<= 1;
		byte |= (__iic_get_sda() & 1);

		__iic_set_scl(0);
		wait_us(5);
	}

	__iic_send_bit(coolman == 0x00);

	return byte;
}

bool __iic_send_byte(uint8_t byte)
{
	for(int i = 7; i >= 0; i--)
	{
		__iic_send_bit(((byte >> i) & 1));
	}

	__iic_set_scl(1);
	wait_us(4);

	bool data_good = (__iic_get_sda() == 0);

	__iic_set_scl(0);
	wait_us(5);

	return data_good;
}

void __iic_stop()
{
	__iic_set_sda(0);
	wait_us(1);

	__iic_set_scl(1);
	wait_us(4);

	__iic_set_sda(1);
	wait_us(5);
}

void iic_init()
{
	__iic_start();
}

uint8_t iic_read_byte(uint8_t node_address, uint8_t data_offset)
{
	uint8_t byte = 0x00;

	__iic_start();

  if(__iic_send_byte(node_address & 0xfe) && __iic_send_byte(data_offset))
  {
	__iic_start();

	if(__iic_send_byte(node_address | 0x01))
	{
		byte = __iic_read_byte(0x00);
	}
  }

  __iic_stop();

  return byte;
}

void iic_write_byte(uint8_t node_address, uint8_t data_offset, uint8_t data)
{
	__iic_start();

	if(__iic_send_byte(node_address & 0xfe) && __iic_send_byte(data))
	{
		__iic_send_byte(data_offset);
	}

	__iic_stop();
}

void iic_close()
{
	__iic_stop();
}
