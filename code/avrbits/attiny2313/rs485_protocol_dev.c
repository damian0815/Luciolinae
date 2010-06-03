#include "Tlc5940_purec/Tlc5940_purec.h"
#include <util/delay.h>
#include <avr/interrupt.h>

#define RS485_TRANSMIT_ENABLE_PIN PA0
#define RS485_TRANSMIT_ENABLE_PORT PORTA
#define RS485_TRANSMIT_ENABLE_DDR DDRA

// message addressing
// a message is constructed as
// <id|func> <data>...
// id of 0 means all boards

// functions
// set all leds on all slaves to this brightness
#define FUNC_SET_ALL 0x01
#define FUNC_SET_ALL_NO_LATCH 0x07 // non-latching option
// set single led on single slave to this brightness
#define FUNC_SET_SINGLE 0x02
#define FUNC_SET_SINGLE_NO_LATCH 0x08 // non-latching version
// pulse a single LED on a single slave
#define FUNC_PULSE_SINGLE 0x03
// latch data into the tlc
#define FUNC_LATCH 0x04
// set some values
#define FUNC_SET_SOME 0x05
// set every value
#define FUNC_SET_EVERY 0x06

unsigned char msg_lengths[0x10] = { 0, 1, 2, 3, 0, /*complicated*/0, 24, 1, 2, /* dummy */0, 0, 0, 0, 0, 0, 0 };

// first byte in EEPROM contains id
// valid board ids are: 0x10, 0x20, 0x30, .. 0xf0
#define EEPROM_ID_ADDRESS 0x00


inline void initUSART( int ubrr )
{

	// calculate value for ubrr
	//unsigned int ubrr = F_OSC/(16*baud) - 1;

	/* Set baud rate */ 
	UBRRH = (unsigned char)(ubrr>>8); 
	UBRRL = (unsigned char)ubrr; 
	/* Enable receiver and transmitter */ 
	UCSRB = (1<<RXEN)|(1<<TXEN); 

	/* Set frame format: 8data, 1stop bit, no parity */ 
	UCSRC = (0<<USBS)|(3<<UCSZ0);

	/* clear RXC bit */
	UCSRA &= ~_BV(RXC);

}


/// check if there is data waiting to be read
/// at 2400 bps, this should be called at least once every 4.2 ms to avoid missing data
unsigned char USART_IsDataWaiting(void)
{
	// check RXC bit
	return ( UCSRA & (1<<RXC) );
}

unsigned char USART_Receive( void ) 
{
	/* Wait for data to be received */ 
	while ( !(UCSRA & (1<<RXC)) )
			; 
	/* Get and return received data from buffer */ 
	return UDR;
	}

/*void waitForTxc()
{
	while ( ! (UCSRA & _BV(TXC)) )
		;	
}*/

unsigned long int millis_counter = 0;
ISR(TIMER1_COMPA_vect)
{
	millis_counter++;
}


unsigned char CRC8(unsigned char input, unsigned char seed)
{
    unsigned char i, feedback;

    for (i=0; i<8; i++)
    {
        feedback = ((seed ^ input) & 0x01);
        if (!feedback) seed >>= 1;
        else
        {
            seed ^= 0x18;
            seed >>= 1;
            seed |= 0x80;
        }
        input >>= 1;
    }

    return seed;   
}


//unsigned char decrement[16];

int main(void)
{
	unsigned char my_id;

	// setup RS484 transmit enable pin
	RS485_TRANSMIT_ENABLE_DDR |= _BV(RS485_TRANSMIT_ENABLE_PIN);
	// turn it off
	RS485_TRANSMIT_ENABLE_PORT &= ~_BV(RS485_TRANSMIT_ENABLE_PIN);

	// ubrr value is CLOCK_FREQUENCY/(16*baud) - 1
	// at 8MHz 1666 gives 300 baud
	//   300: 1666
	//  2400:  207
	//  9600:   51
	// 19200:   25
	initUSART( 25 );
	//initUSART( 51 );
	//initUSART( 1666 );

	// turn on interrupts globally
	sei();
	// init tlc
	tlcClass_init();
	tlcClass_setAll(0);

	// turn on 'milliseconds' timer
	// this will fire the interrupt (TIM1_COMPA_vect)
	// twice for every BLANK: once at the start, and once at the end.
	// since PWM runs at (F_OSC/(2*TLC_PWM_PERIOD) == 488Hz for the 
	// default value of TLC_PWM_PERIOD of 8192, this means that a 
	// timer increment in the TIMER1_COMPA interrupt should happen 
	// about equivalent to once every .976ms, with some wonkiness,
	// which is close enough.
	TIMSK |= _BV(OCIE1A);
	


	// read address from EEPROM	
	/* Set up address register */ 
	EEAR = EEPROM_ID_ADDRESS; 
	/* Start eeprom read by writing EERE */ 
	EECR |= (1<<EERE); 
	/* Return data from data register */ 
	my_id = EEDR;

	// turn on TXC interrupt
	//UCSRB |= _BV(TXCIE);

	unsigned long int prev_millis = millis_counter;
	while(1)
	{
		/*
		unsigned long int now = millis_counter;
		unsigned long int elapsed_millis;
		if ( now < prev_millis )
			// overflow
			elapsed_millis = 1;
		else
			elapsed_millis = now-prev_millis;
		prev_millis = now;	
		// how much to decrement?
		//unsigned int dec = elapsed_millis*6;
		// decrement
		for ( int i=0; i<16; i++ )
		{
			unsigned long int current = tlcClass_get(i);
			// floating point math: current = current-current*elapsed*decay_pct
			// this would mean that every second, current would lose decay_pct of its value
			//
			// fixed point math: current is *2^0, elapsed is *2^-10 (seconds), decay is *2^-4
			// we have 32 bits of precision in an unsigned long int, so this is ok
			unsigned long int next = current + current*elapsed*decay;
			// shift to correct for signs
			// -10+-4 = -14
			next >>= 14; 
			tlcClass_set( i, next );
		}
	*/


		// check for USART
		//
		if ( USART_IsDataWaiting() )
		{
			// get command and board id
			unsigned char command_and_id = USART_Receive();
			unsigned char which_levelhi, levello;
			unsigned char latch = 1;
			unsigned char dec;
			unsigned char count;
			unsigned char msg_buffer[24];
			unsigned char crc;
			int not_for_me = !(((command_and_id&0xf0)==0) || ((command_and_id&0xf0)==my_id) ); 
			// RS485 protocol begins here
			unsigned char cmd = (command_and_id&0x0f);
			
			// fetch bytes, do CRC check
			crc = 0;
			crc = CRC8( command_and_id, crc );
			int length = msg_lengths[cmd];
			// fetch  ,upaditng crc 
			for ( int i=0; i<length; i++ )
			{
				msg_buffer[i] = USART_Receive();
				crc = CRC8( msg_buffer[i], crc );
			}
			// fetch crc and check
			if ( USART_Receive() != crc )
			{
				//tlcClass_setAll( 1024 );
				continue;
			}

			// go
			switch( cmd )
			{
				case FUNC_SET_ALL_NO_LATCH:
					latch = 0;
				case FUNC_SET_ALL: /* set all led's function (8-bit precision)*/ 
					// function is: (board_id|0x01) (level,8bit precision) 
					levello = msg_buffer[0];

					// my board?
					if ( not_for_me )
						// not me
						break;

					tlcClass_setAll( ((unsigned int)levello)<<4 );
					// latch
					if ( latch )
						goto LATCH;
					break;

				case FUNC_SET_SINGLE_NO_LATCH:
					latch = 0;
				case FUNC_SET_SINGLE: /* set led function */ 
					// function is: (board_id|0x02) (which<<4|levelhi) levello
					// which says which LED,
					// levelhi is the top 4 bits of the level
					// levello is the bottom 8 bits of the level
					// latch: 1 is instant, 0 is later
					which_levelhi = msg_buffer[0];
					levello = msg_buffer[1];

					// not for me?
					if ( not_for_me )
						break;

					// for me: go
					tlcClass_set( /* unpack which */ (which_levelhi & 0xf0)>>4, 
							/* unpack level */ (((unsigned int)(which_levelhi & 0x0f))<<8) + levello );
					//decrement[(which_levelhi & 0xf0)>>4] = 0;
					// latch?
					if ( latch )
						goto LATCH;
					break;

				case FUNC_SET_EVERY: /* set every led function */
					// message is: (board|FUNC_SET_EVERY) (24 bytes packed data) 
					// does not latch
					
					// not for me?
					for ( int i=0; i<8; i++ )
					{
						// only use the data if it's for me
						unsigned char data0 = msg_buffer[i*3];
						unsigned char data1 = msg_buffer[i*3+1];
						unsigned char data2 = msg_buffer[i*3+2];
						if ( !not_for_me )
						{
							tlcClass_set( i*2,   (((unsigned int)data0)<<4) | (unsigned int)((data1&0xf0)>>4) );
							tlcClass_set( i*2+1, (((unsigned int)data1&0x0f)<<8) | (unsigned int)data2 );
						}
					}
				
					break;

				case FUNC_SET_SOME:
					// message is: (board|FUNC_SET_SOME) count (count times [(which<<4|levelhi) levello]) 
					// does not latch
				
					count = msg_buffer[0];
					while( count > 0 )
					{
						// decrement counter
						--count;

						// fetch data
						which_levelhi = msg_buffer[0];
						levello = msg_buffer[1];

						// not for me?
						if ( not_for_me )
							continue;

						// for me: go
						tlcClass_set( /* unpack which */ (which_levelhi & 0xf0)>>4, 
								/* unpack level */ (((unsigned int)(which_levelhi & 0x0f))<<8) + levello );
	
					}

					break;
	

					
				case FUNC_PULSE_SINGLE: // pulse led function 
					// function is: (FUNC_PULSE_SINGLE|board_id) (which<<4|levelhi) levello dec
					// which says which LED,
					// levelhi is the top 4 bits of the level
					// levello is the bottom 8 bits of the level
					// dec is the decrement speed
					which_levelhi = msg_buffer[0];
					levello = msg_buffer[1];
					dec = msg_buffer[2];

					if ( not_for_me )
						break;
					tlcClass_set( (which_levelhi & 0xf0)>>4, // unpack which
							(((unsigned int)(which_levelhi & 0x0f))<<8) + levello ); // unpack level
					//decrement[(which_levelhi & 0xf0)>>4] = dec;
					break;

					
				case FUNC_LATCH: /* latch data into the TLC */
					// skip if it's not fur me
					if ( not_for_me )
						break;
LATCH:
					// now latch
					while( tlcClass_update() )
						;
					break;

				default:
					break;
			}
		}

	}

}



