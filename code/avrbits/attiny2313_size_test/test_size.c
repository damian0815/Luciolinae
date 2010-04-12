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

int main(void)
{
	int onOff = 1;

	// pulsing pin for testage
	DDRB |= _BV(PB5); 
	DDRB |= _BV(PB6);

	// turn on interrupts globally
	sei();
	tlcClass_init();
	
	int value1 = 0;
	int value2 = 0;
	int value3 = 0;
	
	while(1)
	{	

		for ( int i=0; i<5; i++ )
			tlcClass_set( i, (value1<3?4095:0) );
		for ( int i=5; i<10; i++ )
			tlcClass_set( i, (value2<7?4095:0) );
		for ( int i=10; i<16; i++ )
			tlcClass_set( i, (value3<5?4095:0) );
		
		while(tlcClass_update()) ;

		value1 += 3;
		value2 += 7;
		value3 += 5;
		if ( value1 >= 4096 )
			value1 -= 4096;
		if ( value2 >= 4096 )
			value2 -= 4096;
		if ( value3 >= 4096 )
			value3 -= 4096;
		
	/*	if ( onOff )
			PORTB |= _BV(PB5);
		else
			PORTB &= ~_BV(PB5);*/
/*		onOff = !onOff;
		if ( onOff )
		{
			turnOnPin18();
		}
		else
		{
			turnOffPin18();
		}*/
		//_delay_us(5);
	}

}

