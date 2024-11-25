#ifndef __LIBWTV_KBINPUT_H
#define __LIBWTV_KBINPUT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// For rand()
#define PHI 0x9e3779b9

/* ------- EMAC LC2 -------- */
#define	kIRSampleDelayEMAC	0x0f
#define	kIRSampleTicksEMAC	0x53

#define	kSejinBitOffMin		0x2cb
#define	kSejinBitOffMax		0x3e3
//
#define	kSejinBitOnMin		0x22b
#define	kSejinBitOnMax		0x32a

#define	kSejinHalfBitcell	0x154
#define	kSejinBitcell		0x303


#define	kTGuideMin			0x7d0
#define	kTGuideMax			0xadc
#define	kTOffMin			0x140
#define	kTOffMax			0x2ee
#define	kTOnZeroMin			0x1c2
#define	kTOnZeroMax			0x370
#define	kTOnOneMin			0x41a
#define	kTOnOneMax			0x5c8
#define	kTInter20Min		0x1f40
#define	kTInter20Max		0x6590

#define	kIRBufSize					64
#define	kIRBufMask					0x3f

#define	kSejinIDMask		0x30
#define	kSejinKeyboardID	0x10

#define	kSejinMakeBreakMask	0x40			/* 0 = MAKE, 1 = BREAK */

#define kIRKeyMapTableSize	128

typedef struct {
	uint8_t kbData;
	uint8_t category1;
	uint8_t category2;
	uint8_t button;
	uint32_t time;
} rawIR;

void init_rand(uint32_t x);

uint32_t rand_cmwc();

void keyboard_init();

void ValidateSejinDataEMAC();

void SejinGetBitEMAC(uint32_t irbit);

void SejinTestBitTime_0EMAC(uint32_t irbit, uint32_t irtime, uint32_t a, uint32_t b);

void SejinTestBitTime_1EMAC(uint32_t irbit, uint32_t irtime, uint32_t a, uint32_t b);

void DecodeSejinEMAC(uint32_t irbit, uint32_t irtime);

void poopieit();

void keyboard_poll();

#ifdef __cplusplus
}
#endif

#endif
