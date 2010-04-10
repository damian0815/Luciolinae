/*
 *  tlc5940.c
 *  tlc5940_smaller
 *
 *  Created by damian on 09/04/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "tlc5940.h"


// use any two pins
#define TLC_BITBANG	1
// use hardware SPI pins only
#define TLC_SPI		2
// which transfer mode?
#define DATA_TRANSFER_MODE TLC_SPI

// how many TLC5940 devices?
#define NUM_TLCS	1




// clock stuff

/* 
 PWM period in clocks

 to convert to frequency:
   f_pwm = f_osc/(2*TLC_PWM_PERIOD)
 so
   TLC_PWM_PERIOD = f_osc/(2*f_pwm)
 
 f_osc      TLC_PWM_PERIOD		f_pwm
  8MHz			8192			488.28125 Hz
  8MHz			4096			976.5625 Hz
 16MHz			8192			976.5625 Hz 
 
 for a 8MHz clock, setting this to 8192 gives 
 a PWM frequency of ~490kHz PWM.
*/
#define TLC_PWM_PERIOD 8192

/*
 GSCLK period in clocks
 
 related to TLC_PWM_PERIOD:
 TLC_GSCLK_PERIOD = (2*TLC_PWM_PERIOD)/4096 - 1
 
 TLC_PWM_PERIOD			TLC_GSCLK_PERIOD
	16384					7
	 8192					3
	 4096					1
 */
#define TLC_GSCLK_PERIOD ((2*TLC_PWM_PERIOD)/4096-1)

// needed for dot-correction
#define VPRG_ENABLED 0
// to get error information about LEDs
#define XERR_ENABLED 0




// nice looking macros for pin stuff
#define pinMask(pinId) (_BV(pinId));
#define pinMask(pinId1,pinId2) (_BV(pinId1) & _BV(pinId2))
#define pinMask(pinId1,pinId2,pinId3) (_BV(pinId1) & _BV(pinId2) & _BV(pinId3))

// convenience; DDR or PIN
#define pinHigh(port,pin) (port |= pinMask(pin))
#define pinLow(port,pin) (port &= ~pinMask(pin))
#define ddrSetWrite(ddr,pin) (ddr |= pinMask(pin))
#define ddrSetRead(ddr,pin) (ddr &= ~pinMask(pin))

// pulse a pin high then low
#define pinPulse(port, pin)   port |= pinMask(pin); port &= pinMask(pin)








// grayscale data
uint8_t tlc_data[NUM_TLCS*24];



#if DATA_TRANSFER_MODE == TLC_BITBANG

/** SIN (TLC pin 26) */
#define SIN_PIN        DEFAULT_BB_SIN_PIN
#define SIN_PORT       DEFAULT_BB_SIN_PORT
#define SIN_DDR        DEFAULT_BB_SIN_DDR
/** SCLK (TLC pin 25) */
#define SCLK_PIN       DEFAULT_BB_SCLK_PIN
#define SCLK_PORT      DEFAULT_BB_SCLK_PORT
#define SCLK_DDR       DEFAULT_BB_SCLK_DDR

// Set all the bit-bang pins to output
void tlc_shift8_init(void)
{
	ddrSetWrite(  SIN_DDR, SIN_PIN );
	ddrSetWrite( SCLK_DDR, SCLK_PIN );
	pinLow( SCLK_PORT, SCLK_PIN );
}

// Shifts a byte out, MSB first 
void tlc_shift8(uint8_t byte)
{
    for (uint8_t bit = 0x80; bit; bit >>= 1) 
	{
		// if we have a 1 in this position
        if (bit & byte) 
		{
			// turn on SIN_PIN
			pinHigh(SIN_PORT, SIN_PIN);
        } else 
		{
			// turn off SIN_PIN
			pinLow(SIN_PORT, SIN_PIN);
        }
		// pulse clock pin
        pinPulse(SCLK_PORT, SCLK_PIN);
    }
}

#elif DATA_TRANSFER_MODE == TLC_SPI

/** SIN (TLC pin 26) */
#define SIN_PIN        DEVICE_MOSI_PIN
#define SIN_PORT       DEVICE_MOSI_PORT
#define SIN_DDR        DEVICE_MOSI_DDR
/** SCLK (TLC pin 25) */
#define SCLK_PIN       DEVICE_SCK_PIN
#define SCLK_PORT      DEVICE_SCK_PORT
#define SCLK_DDR       DEVICE_SCK_DDR
#endif

/** Initializes the SPI module to double speed (f_osc / 2) */
void tlc_shift8_init(void)
{
	// SPI MOSI, SPI SCK and SPI SS as output
	ddrSetWrite(  SIN_DDR, SIN_PIN );
	ddrSetWrite( SCLK_DDR, SCLK_PIN );
	ddrSetWrite(   SS_DDR, SS_PIN );
	pinLow( SCLK_PORT, SCLK_PIN );

	// double speed ( f_osc/2 )
	SPSR = _BV(SPI2X);
	// enable SPI as master
	SPCR = _BV(SPE) | _BV(MSTR);	
}


/** Shifts out a byte, MSB first */
void tlc_shift8(uint8_t byte)
{
    SPDR = byte; // starts transmission
    while (!(SPSR & _BV(SPIF)))
        ; // wait for transmission complete
}

#else
#error "invalid DATA_TRANSFER_MODE"
#endif


void tlc_init()
{
	// set write on XLAT, BLANK, GSCLK 
	ddrSetWrite( XLAT_DDR, XLAT_PIN );
	ddrSetWrite( BLANK_DDR, BLANK_PIN );
	ddrSetWrite( GSCLK_DDR, GSCLK_PIN );
	
#if VPRG_ENABLED
	// set direction to write
	ddrSetWrite ( VPRG_DDR, VPRG_PIN );
	// set low
	pinLow( VPRG_PORT, VPRG_PIN );
#endif
#if XERR_ENABLED
	// XERR as input
	ddrSetRead( XERR_DDR, XERR_PIN ); 
	// enable pull-up resistor
	pinHigh( XERR_PORT, XERR_PIN );	  
#endif

	// set blank high (until the timers start)
	pinHigh( BLANK_PORT, BLANK_PIN );

	tlc_shift8_init();

	// set all to 0
	for ( int i=0; i<16; i++ )
		tlc_set(i,0);
	
	// push out to device
	tlc_sendToDevice();
	// .. but disable pulses
    disable_XLAT_pulses();
	// .. and interrupt
    clear_XLAT_interrupt();
	// .. we don't need it
    tlc_needXLAT = 0;
	// puls XLAT to latch in initial data
    pulse_pin(XLAT_PORT, XLAT_PIN);
	
	// setup timers..
	
    // Timer 1 - BLANK / XLAT
    TCCR1A = _BV(COM1B1);  // non inverting, output on OC1B, BLANK
    TCCR1B = _BV(WGM13);   // Phase/freq correct PWM, ICR1 top
    OCR1A = 1;             // duty factor on OC1A, XLAT is inside BLANK
    OCR1B = 2;             // duty factor on BLANK (larger than OCR1A (XLAT))
    ICR1 = TLC_PWM_PERIOD; // see tlc_config.h
	
    // Timer 2 - GSCLK
#ifdef TLC_ATMEGA_8_H
    TCCR2  = _BV(COM20)       // set on BOTTOM, clear on OCR2A (non-inverting),
		   | _BV(WGM21);      // output on OC2B, CTC mode with OCR2 top
    OCR2   = TLC_GSCLK_PERIOD / 2; // see tlc_config.h
    TCCR2 |= _BV(CS20);       // no prescale, (start pwm output)
#else
    TCCR2A = _BV(COM2B1)      // set on BOTTOM, clear on OCR2A (non-inverting),
							  // output on OC2B
		   | _BV(WGM21)       // Fast pwm with OCR2A top
		   | _BV(WGM20);      // Fast pwm with OCR2A top
    TCCR2B = _BV(WGM22);      // Fast pwm with OCR2A top
    OCR2B = 0;                // duty factor (as short a pulse as possible)
    OCR2A = TLC_GSCLK_PERIOD; // see tlc_config.h
    TCCR2B |= _BV(CS20);      // no prescale, (start pwm output)
#endif
    TCCR1B |= _BV(CS10);      // no prescale, (start pwm output)
	// send again?
    tlc_sendToDevice();
	
	
}


/// returns 1 if send succeeded, or 0 if we're still waiting for 
/// earlier data to be latched in
int tlc_sendToDevice()
{
	if (tlc_needXLAT) {
		// still waiting for an XLAT
        return 1;
    }
	
	// don't send XLAT pulses any more
    tlc_disableXLATPulses();
 
	/*
	 // if we add dc correction stuff it goes here
    if (firstGSInput) {
        // adds an extra SCLK pulse unless we've just set dot-correction data
        firstGSInput = 0;
    } else {
        pulse_pin(SCLK_PORT, SCLK_PIN);
    }*/
	
	// shift out all the data
    uint8_t *p = tlc_data;
    while (p < tlc_data + NUM_TLCS * 24) {
        tlc_shift8(*p++);
        tlc_shift8(*p++);
        tlc_shift8(*p++);
    }
	// we need XLAT again
    tlc_needXLAT = 1;
	tlc_enableXLATPulses();
    tlc_setXLATInterrupt();
	
    return 0;
}	


// Interrupt called after an XLAT pulse to prevent more XLAT pulses
ISR(TIMER1_OVF_vect)
{
    tlc_disableXLATPulses();
    tlc_clearXLATInterrupt();
    tlc_needXLAT = 0;
}



