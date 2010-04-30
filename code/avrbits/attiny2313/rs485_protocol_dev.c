#include "Tlc5940_purec/Tlc5940_purec.h"
#include <util/delay.h>
#include <avr/interrupt.h>


// depends on clock freq: this is for 8MHz
/*void delay_ms(long unsigned int ms)
{
	long unsigned int i;
	while( ms != 0 ) 
	{
		for ( i=0;i <4200; i++ )
			;
		ms--;
	}
}*/

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


#define RS485_TRANSMIT_ENABLE_PIN PA0
#define RS485_TRANSMIT_ENABLE_PORT PORTA
#define RS485_TRANSMIT_ENABLE_DDR DDRA

int main(void)
{
	// ubrr value is CLOCK_FREQUENCY/(16*baud) - 1
	// at 8MHz 1666 gives 300 baud
	//   300: 1666
	//  2400:  207
	//  9600:   51
	// 19200:   25
	initUSART( 25 );
	//initUSART( 1666 );

	// turn on interrupts globally
	sei();
	// init tlc
	tlcClass_init();
	tlcClass_setAll(0);

	// turn on TXC interrupt
	//UCSRB |= _BV(TXCIE);

	// turn on PD3,PD4 for output (pin 7,8)
	DDRD |= _BV(PD4);
	DDRD |= _BV(PD3);
	// turn off the pins	
	PORTD &= ~_BV(PD4);
	PORTD &= ~_BV(PD3);

	// setup RS484 transmit enable pin
	RS485_TRANSMIT_ENABLE_DDR |= _BV(RS485_TRANSMIT_ENABLE_PIN);
	// turn it off
	RS485_TRANSMIT_ENABLE_PORT &= ~_BV(RS485_TRANSMIT_ENABLE_PIN);

	while(1)
	{
		// check for USART
		//
		if ( USART_IsDataWaiting() )
		{
			unsigned char command = USART_Receive();
			// RS485 protocol begins here
			if ( command == 0x01 /* set all led's function (8-bit precision)*/ )
			{
				// function is: 0x01 (level as a byte)
				unsigned char level_8bit = USART_Receive();
				tlcClass_setAll( ((unsigned int)level_8bit)<<4 );
				while( tlcClass_update() )
					;
			}
			else if ( command == 0x02 /* set led function */ )
			{
				// function is: 0x02 (which<<4|levelhi) levello 
				// which says which LED,
				// levelhi is the top 4 bits of the level
				// levello is the bottom 8 bits of the level
				unsigned char mask, which_levelhi, levello;
				// results of unpacking
				unsigned char which;
				unsigned int level;
				which_levelhi = USART_Receive();
				levello = USART_Receive();
			
				// unpack which
				which = (which_levelhi & 0xf0)>>4;
				// unpack level
				level = 16*(which_levelhi & 0x0f) + levello;
					
				tlcClass_set( which, level );
				while( tlcClass_update() )
					;
			}
		}

	}

}



