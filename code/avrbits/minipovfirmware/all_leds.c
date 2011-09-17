#include <avr/io.h>      // this contains all the IO port definitions

int main(void) {

  DDRB = 0xFF;       // set all 8 pins on port B to outputs

  PORTB = 0xFF;      // set all the pins on port B high (turn on those LEDs)

  while (1);         // basically halt, do nothing forever
}
