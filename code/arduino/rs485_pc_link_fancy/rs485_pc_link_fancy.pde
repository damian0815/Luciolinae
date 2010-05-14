#include <NewSoftSerial.h>

// Serial is for the USB connection
// rs485_serial is the for RS485 connection

// size of bytes to wait until we forward to rs485
#define CHUNKSIZE 4

// size of transferred blocks, in byte
// sends an 'ack' every this-many bytes
#define BLOCKSIZE 128

// setup new soft serial on pins 3 & 4
NewSoftSerial rs485_serial( 3,4 ); // rx = 3, tx = 4
#define RS485_TRANSMIT_ENABLE_PIN 5

void setup()
{
  pinMode(RS485_TRANSMIT_ENABLE_PIN, OUTPUT);
  digitalWrite(RS485_TRANSMIT_ENABLE_PIN, LOW);
  
  Serial.begin( 9600 );
  rs485_serial.begin( 9600 );
  
  // pulse led's to say we're ready
  byte msg[2];
  msg[0] = 0x01;
  msg[1] = 0x80;
  rs485Write( msg, 2 );
  delay( 100 );
  msg[1] = 0x00;
  rs485Write( msg, 2 );
}

void rs485Write( byte* data, int count )
{
  // write char-by-char to rs485 serial interface
  digitalWrite(RS485_TRANSMIT_ENABLE_PIN, HIGH);
  for( int i=0; i<count; i++ )
  {
    rs485_serial.print( data[i], BYTE );
  }
  delayMicroseconds(1800);
  digitalWrite( RS485_TRANSMIT_ENABLE_PIN, LOW );
}

int r_count = 0;
void loop()
{
  // mirror USB serial in to rs485, in 8 byte chunks
  if( Serial.available() >= CHUNKSIZE )
  {
    
    byte data[CHUNKSIZE];
    for ( int i=0; i<CHUNKSIZE; i++ )
    {
      data[i] = Serial.read();
      // echo back with a _ before (to show we sent it)
      //Serial.print( "_" );
      //Serial.print( (byte)data[i], HEX );
    }
    // write rs 485
    rs485Write( data, CHUNKSIZE );
    // ack?
    r_count += CHUNKSIZE;
    if ( r_count >= BLOCKSIZE )
    {
      // ack
      Serial.print((byte)0x7f, BYTE );
      r_count -= BLOCKSIZE;
    }
  }
  // mirror rs485 serial out to USB
  while( rs485_serial.available() > 0 )
  {
    Serial.print(" ");
    Serial.print( (byte)rs485_serial.read(), HEX );
  }
}

