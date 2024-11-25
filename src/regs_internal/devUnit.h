/**
 * @file devUnit.h
 * @ingroup hardware
 * @brief WebTV's I/O device unit interface
 */

#include <stdint.h>

// Front panel LEDs

#define DEV_LED                0xa4004004

// SSID chip

#define DEV_ID_CNTL            0xa4004008

// IIC (I2C) bus

#define DEV_IIC_CNTL           0xa400400c

// GPIO

#define DEV_GPIO_IN            0xa4004010

#define DEV_GPIO_OUT_SET       0xa4004014
#define DEV_GPIO_OUT_CLEAR     0xa4004114

#define DEV_GPIO_EN_SET        0xa4004018
#define DEV_GPIO_EN_CLEAR      0xa4004118

// IR in and out

#define DEV_IRDATA_OLD         0xa4004000

#define DEV_IR_IN_SAMPLE_TICKS 0xa4004020
#define DEV_IR_IN_REJECT_TIME  0xa4004024
#define DEV_IR_IN_TRANS_DATA   0xa4004028
#define DEV_IR_IN_STAT_CNTL    0xa400402c

#define DEV_IR_OUT_FIFO        0xa4004040
#define DEV_IR_OUT_STATUS      0xa4004044
#define DEV_IR_OUT_PERIOD      0xa4004048
#define DEV_IR_OUT_ON          0xa400404c
#define DEV_IR_OUT_CUR_PERIOD  0xa4004050
#define DEV_IR_OUT_CUR_ON      0xa4004054
#define DEV_IR_OUT_CUR_COUNT   0xa4004058

// Parallel port

#define DEV_PPORT_DATA         0xa4004200
#define DEV_PPORT_CTRL         0xa4004204
#define DEV_PPORT_STAT         0xa4004208
#define DEV_PPORT_CNFG         0xa400420c
#define DEV_PPORT_FIFO_CNTL    0xa4004210
#define DEV_PPORT_FIFO_STAT    0xa4004214
#define DEV_PPORT_FIFO_TIMEOUT 0xa4004218
#define DEV_PPORT_STAT2        0xa400421c
#define DEV_PPORT_INT_EN       0xa4004220
#define DEV_PPORT_INT_STAT     0xa4004224
#define DEV_PPORT_INT_CLEAR    0xa4004228
#define DEV_PPORT_ENABLE       0xa400422c

// ISA device

#define DEV_CNTL0              0xa400100c
#define DEV_CNTL1              0xa4001010

// Hardware keyboard on SPOT boxes (overlaps IR out on SOLO boxes)

#define DEV_KBD_BASE           0xa4004020

#define KBD0                   0x00
#define KBD1                   0x04
#define KBD2                   0x08
#define KBD3                   0x0c
#define KBD4                   0x10
#define KBD5                   0x14
#define KBD6                   0x18
#define KBD7                   0x1c

#define DEV_KBD0               (DEV_KBD_BASE + KBD0)
#define DEV_KBD1               (DEV_KBD_BASE + KBD1)
#define DEV_KBD2               (DEV_KBD_BASE + KBD2)
#define DEV_KBD3               (DEV_KBD_BASE + KBD3)
#define DEV_KBD4               (DEV_KBD_BASE + KBD4)
#define DEV_KBD5               (DEV_KBD_BASE + KBD5)
#define DEV_KBD6               (DEV_KBD_BASE + KBD6)
#define DEV_KBD7               (DEV_KBD_BASE + KBD7)

// External timing control on SPOT boxes (overlaps GPIO out on SOLO boxes)

#define DEV_EXT_TIMING         0xa4004014

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

#define LED_POWER              0x00000004
#define LED_CONNECTED          0x00000002
#define LED_MESSAGE            0x00000001
#define LEDS_OFF               0x00000000
#define LEDS_ALL               LED_POWER | LED_CONNECTED | LED_MESSAGE

#define IR_STATUS_IN_INTTHRESH(entry_count) ((entry_count & 0xf) << 4)
#define IR_STATUS_IN_DEBUG     0x04
#define IR_STATUS_IN_RESET     0x02
#define IR_STATUS_IN_ENABLED   0x01
