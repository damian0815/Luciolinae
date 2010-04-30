/*  Copyright (c) 2009 by Alex Leone <acleone ~AT~ gmail.com>

    This file is part of the Arduino TLC5940 Library.

    The Arduino TLC5940 Library is free software: you can redistribute it
    and/or modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    The Arduino TLC5940 Library is distributed in the hope that it will be
    useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with The Arduino TLC5940 Library.  If not, see
    <http://www.gnu.org/licenses/>. */

#ifndef TLC_ATTINY2313_H
#define TLC_ATTINY2313_H

#if DATA_TRANSFER_MODE == TLC_SPI
//#warning SPI cannot be used on the ATtiny2313 because I dont understand how
//#warning setting DATA_TRANSFER_MODE to TLC_BITBANG
#undef DATA_TRANSFER_MODE
#define DATA_TRANSFER_MODE  TLC_BITBANG
#endif

/** \file
    SPI and timer pins for the ATtiny2313.  Don't edit these.  All
    changeable pins are defined in tlc_config.h */

/** Pin 14 (OC0A/PCINT2) -> VPRG (TLC pin 27) */
#define DEFAULT_VPRG_PIN    PB2
#define DEFAULT_VPRG_PORT   PORTB
#define DEFAULT_VPRG_DDR    DDRB

/** Pin 17 (MOSI/DI/SDA/PCINT5) -> XERR (TLC pin 16) */
#define DEFAULT_XERR_PIN    PB5
#define DEFAULT_XERR_PORT   PORTB
#define DEFAULT_XERR_DDR    DDRB
#define DEFAULT_XERR_PINS   PINB

/** Pin 14 (OC0A/PCINT2) -> SCLK (TLC pin 25) */
#define DEFAULT_BB_SCLK_PIN     PB2
#define DEFAULT_BB_SCLK_PORT    PORTB
#define DEFAULT_BB_SCLK_DDR     DDRB
/** Pin 13 (AIN1/PCINT1) -> SIN (TLC pin 26) */
#define DEFAULT_BB_SIN_PIN      PB1
#define DEFAULT_BB_SIN_PORT     PORTB
#define DEFAULT_BB_SIN_DDR      DDRB

/** Pin 19 (USCK/SCL/PCINT7) -> SCLK (TLC pin 25) */
#define TLC_SCK_PIN      PB7
#define TLC_SCK_PORT     PORTB
#define TLC_SCK_DDR      DDRB
/** Pin 18 (MOSI/DI/SDA/PCINT5) -> SIN (TLC pin 26) */
#define TLC_MOSI_PIN     PB6
#define TLC_MOSI_PORT    PORTB
#define TLC_MOSI_DDR     DDRB

/** SS will be set to output as to not interfere with SPI master operation.
    If you have changed the pin-outs and the library doesn't seem to work
    or works intermittently, make sure this pin is set correctly.  This pin
    will not be used by the library other than setting its direction to
    output. */
//#define TLC_SS_PIN       PB2
//#define TLC_SS_DDR       DDRB

/** OC1A (Pin 15) -> XLAT (TLC pin 24) */
#define XLAT_PIN     PB3
#define XLAT_PORT    PORTB
#define XLAT_DDR     DDRB

/** OC1B (Pin 16) -> BLANK (TLC pin 23) */
#define BLANK_PIN    PB4
#define BLANK_PORT   PORTB
#define BLANK_DDR    DDRB

/** OC0B (Pin 9) -> GSCLK (TLC pin 18) */
#define GSCLK_PIN    PD5
#define GSCLK_PORT   PORTD
#define GSCLK_DDR    DDRD

#endif

