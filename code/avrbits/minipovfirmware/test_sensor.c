#include <avr/io.h>      // this contains all the IO port definitions


int main(void) {

  DDRB = 0xFF;        // set port B to output only
  PORTB = 0;          // turn off all LEDs

  DDRD = 0xFB;        // one input on pin D2
  PORTD = 0x04;       // turn on pullup on pin D2

  while (1) {
    if (PIND & 0x4) {        // if the sensor switch is off...
      PORTB = 0x0;
    } else {                 // if the sensor switch is on...
      PORTB = 0x1;
    }
  }
}
