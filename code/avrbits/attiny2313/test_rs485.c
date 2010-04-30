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

#define DO_TLC_STUFF
// #define DO_BLINK

#define CLOCK_FREQUENCY (8*(10^6))

inline void initUSART( int ubrr )
{

	// calculate value for ubrr
	//unsigned int ubrr = CLOCK_FREQUENCY/(16*baud) - 1;

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
unsigned char USART_IsDataWaiting()
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

 void waitForTxc()
{
	while ( ! (UCSRA & _BV(TXC)) )
		;	
}

#define RS485_TRANSMIT_ENABLE_PIN PA0
#define RS485_TRANSMIT_ENABLE_PORT PORTA
#define RS485_TRANSMIT_ENABLE_DDR DDRA

int main(void)
{
//	int  onOff;

//	int  count;


//unsigned char onOff;
//	int count;

	// ubrr value is CLOCK_FREQUENCY/(16*baud) - 1
	// at 8MHz 1666 gives 300 baud
	//   300: 1666
	//  2400:  207
	//  9600:   51
	// 19200:   25
	initUSART( 25 );
	//initUSART( 19200 );

	//int onOff = 1;

	// pulsing pin for testage
	//DDRB |= _BV(PB5); 
	//DDRB |= _BV(PB6);

#ifdef DO_TLC_STUFF
	// turn on interrupts globally
	sei();
	// init tlc
	tlcClass_init();
	tlcClass_setAll(0);
#endif


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

	int onOff = 0;
	//count = 500;
	unsigned long count = 0;	

	while(1)
	{
		// check for USART
		//
		if ( USART_IsDataWaiting() )
		{
			unsigned char data = USART_Receive();
			if ( data == 0x01 /* led function */ )
			{
				//PORTD &= ~_BV(PD3);
				// get next byte
				data = USART_Receive();
				if ( data == 0x0f )
				{
					PORTD |= _BV(PD3);
					PORTD &= ~_BV(PD4);
				}
				else if ( data == 0xff )
				{
					PORTD &= ~_BV(PD3);
					PORTD |= _BV(PD4);
				}
				else if ( data == 0x00 )
				{
					PORTD &= ~_BV(PD3);
					PORTD &= ~_BV(PD4);
				}
				else
				{
			/*		// unknown
					PORTD |= _BV(PD3);
					PORTD |= _BV(PD4);
					*/
				}
#ifdef DO_TLC_STUFF
				tlcClass_setAll( data*16 );
				while( tlcClass_update() )
					;
#endif
			}
		/*	else
				PORTD |= _BV(PD3);*/
		}

#ifdef DO_BLINK	
		// blink
		count += 1;
		if ( count > 1000000)
		{
			onOff = !onOff;
			count = 0;
			if ( onOff )
			{
				PORTD |= _BV(PD4);
			}
			else
			{
				PORTD &= ~_BV(PD4);
			}
		}
#endif

		//_delay_ms(3);
	
	}

}



