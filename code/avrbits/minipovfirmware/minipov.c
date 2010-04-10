#include <avr/io.h>      // this contains all the IO port definitions
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <util/delay.h>

void delay_ms( uint16_t milliseconds)
{
   for( ; milliseconds > 0; milliseconds--)
   {
      _delay_ms( 1);
   }
} 

#define TIMER1_PRESCALE_1 1
#define TIMER1_PRESCALE_8 2
#define TIMER1_PRESCALE_64 3
#define TIMER1_PRESCALE_256 4
#define TIMER1_PRESCALE_1024 5

// We use these macros because binary constants arent always supported. ugh.
#define HEX__(n) 0x##n##UL
#define B8__(x) ((x&0x0000000FLU)?1:0)  \
               +((x&0x000000F0LU)?2:0)  \
               +((x&0x00000F00LU)?4:0)  \
               +((x&0x0000F000LU)?8:0)  \
               +((x&0x000F0000LU)?16:0) \
               +((x&0x00F00000LU)?32:0) \
               +((x&0x0F000000LU)?64:0) \
               +((x&0xF0000000LU)?128:0)
#define B8(d) ((unsigned char)B8__(HEX__(d)))

const static int image[] = {
  B8(00111100),
  B8(00000100),
  B8(00111000),
  B8(00000100),
  B8(00111000),
  B8(00000000),
  B8(00111101),
  B8(00000000),
  B8(00111100),
  B8(00000100),
  B8(00000100),
  B8(00111000),
  B8(00000000),
  B8(00111101),
  B8(00000000),
  B8(11111100),
  B8(00100100),
  B8(00100100),
  B8(00011000),
  B8(00000000),
  B8(00011000),
  B8(00100100),
  B8(00100100),
  B8(00011000),
  B8(00000000),
  B8(00000100),
  B8(00111000),
  B8(00010000),
  B8(00001100),
  B8(00000000),
  B8(00101111),
  B8(00000000),
  B8(00000000),
  B8(00000000),
  B8(00000000),
  B8(00000000),
  B8(00000000),
};

#define NUM_ELEM(x) (sizeof (x) / sizeof (*(x)))
int imagesize = NUM_ELEM(image);

// this function is called when timer1 compare matches OCR1A
uint8_t j = 0;
SIGNAL( SIG_TIMER1_COMPA ) {
  if (j >= imagesize) 
    j = 0;

  PORTB = image[j];
 
  j++;
}

int main(void) {

  DDRB = 0xFF;       // set all 8 pins on port B to outputs

  /*
    the frequency of the interrupt overflow is determined by the 
    prescaler and overflow value.
    freq = clock_frequency / ( 2 * prescaler * overflow_val)
    where prescaler can be 1, 8, 64, 256, or 1024
    clock_freq is 8MHz
    and overflow_val is 16bit

    the overflow value is placed in OCR1A, the prescale is set in TCCR1B
    so for example:
    A good POV frequency is around 400Hz
    desired freq = 400Hz
    clock freq = 8MHz
    8MHz / (400Hz * 2) = 10000
    since 10000 is less than 655536 (largest 16 bit number)
    OCR1A = 10000 and the prescale is 1
  */

  TCCR1B = (1 << WGM12) | TIMER1_PRESCALE_1;
  OCR1A = (uint16_t)10000;

  TIMSK |= 1 << OCIE1A;   // Output Compare Interrupt Enable (timer 1, OCR1A) 

  sei();                 // Set Enable Interrupts

  while (1);
}
