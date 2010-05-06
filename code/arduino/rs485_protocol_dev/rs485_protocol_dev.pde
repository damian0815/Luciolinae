
#include <avr/io.h>
#include <avr/interrupt.h>

// the led pin
static const int LED_PIN = 5;
static unsigned char RS485_TRANSMIT_ENABLE_PIN = 2;

// message is 0x01 <led value>
static unsigned char FUNC_LED_SETALL_8BIT = 0x01;
static unsigned char FUNC_LED_SET = 0x02;
static unsigned char FUNC_LED_PULSE = 0x03;

// the buffer for messages
unsigned char msg_buf[8];

void ledTestPattern()
{
  // turn all off, on, off, 50%, off
  sendSetAll( 0 );
  delay( 25 );
  sendSetAll( 255 );
  delay( 25 );
  sendSetAll( 0 );
  delay( 25 );
  sendSetAll( 63 );
  delay( 25 );
  sendSetAll( 0 );
  
  // turn on individually 25, 50, 75, 100%
  for ( unsigned char i=0; i<16; i++ )
  {
    unsigned int level = 511;
    for ( unsigned char j=0; j<8; j++ )
    {
      sendSet( i, level );
      delay ( 5 );
      level += 512;
    }
    sendSet( i, 0 );
    delay( 5 );
  }
  
}

// send the contents of the buffer
void sendBuffer( unsigned int byte_count )
{
  // enable RS485 write
  digitalWrite( RS485_TRANSMIT_ENABLE_PIN, HIGH );
  Serial.write( msg_buf, byte_count );
  waitForTxc();
  delayMicroseconds( 1800 );
  // RS485 done
  digitalWrite( RS485_TRANSMIT_ENABLE_PIN, LOW );
  
}

// set all leds (8 bit precision)
void sendSetAll( unsigned char level_8bit )
{
  msg_buf[0] = FUNC_LED_SETALL_8BIT;
  msg_buf[1] = level_8bit;
  sendBuffer( 2 );
}


// set just one led
void sendSet( unsigned char which, unsigned int level_16bit )
{
  // function is: 0x02 (which<<4 | levelhi) levello 
  // which says which LED,
  // levelhi is the top 4 bits of the level
  // levello is the bottom 8 bits of the level
  unsigned char levelhi, levello;
  levelhi = (level_16bit & 0x0f00) >> 8 ;
  levello = (level_16bit & 0x00ff);
  msg_buf[0] = FUNC_LED_SET;
  msg_buf[1] = which<<4 | levelhi;
  msg_buf[2] = levello;
  sendBuffer( 3 );

}


// pulse just one led
void sendPulse( unsigned char which, unsigned int level_16bit )
{
  // function is: 0x02 (which<<4 | levelhi) levello 
  // which says which LED,
  // levelhi is the top 4 bits of the level
  // levello is the bottom 8 bits of the level
  unsigned char levelhi, levello;
  levelhi = (level_16bit & 0x0f00) >> 8 ;
  levello = (level_16bit & 0x00ff);
  msg_buf[0] = FUNC_LED_PULSE;
  msg_buf[1] = which<<4 | levelhi;
  msg_buf[2] = levello;
  sendBuffer( 3 );

}

volatile void waitForTxc() 
{
  // wait for TXC 
  while( !(UCSR0A & _BV(TXC0) ) )
    ;  
  // clear TXC
  UCSR0A |= _BV(TXC0);
}

//////////////////////////////////////
/////////////////////////////////////

void setup()
{
//  pinMode( LED_PIN, OUTPUT );
  // turn off transmit on the RS485 initially
  pinMode( RS485_TRANSMIT_ENABLE_PIN, OUTPUT );
  digitalWrite( RS485_TRANSMIT_ENABLE_PIN, LOW );

  Serial.begin(9600); 
  
  // run test pattern
  ledTestPattern();
  
  sendSetAll( 255 );
  delay( 2000 );
  sendSetAll( 0 );
}


void loop()
{
  
  /*
  for (unsigned char which=0; which < 16 ; which++ )
  {
    unsigned int level = 4095;
    sendSet( which, level );
    delay( 2000 );
    sendSet( which, 0 );
    delay( 5000 );
  }*/
  
  // pick a random led
  unsigned char which = random() % 0x0f;
  // pick a random level
  unsigned int level = random() % 0x0800 + 0x07ff;
  // go
  sendSet( which, level );
  //sendSet( which, 4095 );
  delay( 20 );
  sendSet( which, 0 );

  // pick a random delay time in the range of DELAY_MIN-DELAY_MAX ms
  // 1,000, milliseconds max -> sqrt is 32
  // 1 microseconds min -> sqrt is 1
  #define SQRT_DELAY_MAX 32
  #define SQRT_DELAY_MIN 1
  unsigned int pause = random(SQRT_DELAY_MIN,SQRT_DELAY_MAX);
  // now wait
  delay( pause*pause );
  
}


