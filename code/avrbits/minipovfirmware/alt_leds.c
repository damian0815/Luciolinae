#include <avr/io.h>      // this contains all the IO port definitions
#include <util/delay.h>

// This function basically wastes time
void delay_ms( uint16_t milliseconds)
{
   for( ; milliseconds > 0; milliseconds--)
   {
      _delay_ms( 1);
   }
} 


int main(void) {

  DDRB = 0xFF;       // set all 8 pins on port B to outputs
  
  PORTB = 0x0;       // set all pins on port B low (turn off LEDs)

  while (1) {
    PORTB = 0xAA;      // turn on LEDs #2,4,6,8

    delay_ms(500);    // delay half a second

    PORTB = 0x55;      // turn on LEDs #1,3,5,7

    delay_ms(500);    // delay half a second
  }
}
