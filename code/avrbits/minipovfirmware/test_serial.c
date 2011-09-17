#include <avr/io.h>      // this contains all the IO port definitions
#include <avr/interrupt.h>

#define DTR PB7
#define CTS PB6

// This function basically wastes time
void delay_ms(long int ms) {
  unsigned long int timer;

  while (ms != 0) {
    // this number is dependant on the clock frequency
    for (timer=0; timer <= 4200; timer++);
    ms--;
  }
}

char uart_getchar(void) {
  while (! (UCSRA & _BV(RXC)) );
  return UDR;
}

void uart_putchar(char d) {
  while ( ! (UCSRA & _BV(UDRE)) );
  UDR = d;
}

void uart_init(void) {
  //UCSRA = _BV(U2X);
  UCSRB = _BV(RXEN) | _BV(TXEN);
  UBRRL = 50; // 9600
  UCSRC = _BV(UCSZ1) | _BV(UCSZ0);
}

int main(void) {
  int i = 0;

  DDRB = 0xDF;        // set port B to output only
  PORTB = 0x80;          // turn off all LEDs

  uart_init();
  while (1) {
    UBRRL = 51; // 9600
    for (i = 0; i< 20; i++) {
      uart_putchar('U');
      delay_ms(10);
    } 

    //    UBRRL++;
  }

   // see if serial port is connected
  DDRB &= ~_BV(DTR); // set DTR to input
  uart_getchar(); uart_putchar('1');
  delay_ms(5);

  if (PINB & _BV(DTR)) {
    // something pulling it up
    i = 1;
  }
  uart_getchar(); uart_putchar('0'+i);


  if (i == 0) {
    PORTB |= _BV(DTR); // set pullup
    delay_ms(5);
    if (! (PINB & _BV(DTR))) {
      // something pulling it down
      i = 1;
    }
  }
  if (i == 1) {
  } else {
  }

  uart_putchar('>');

  while (1) {
    //    delay_ms(100);
  }
}
