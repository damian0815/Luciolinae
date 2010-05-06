#include <NewSoftSerial.h>

// Serial is for the USB connection
// rs485_serial is the for RS485 connection


// setup new soft serial on pins 3 & 4
NewSoftSerial rs485_serial( 3,4 ); // rx = 3, tx = 4
#define RS485_TRANSMIT_ENABLE_PIN 5

void setup()
{
  pinMode(RS485_TRANSMIT_ENABLE_PIN, OUTPUT);
  digitalWrite(RS485_TRANSMIT_ENABLE_PIN, LOW);
  
  Serial.begin( 9600 );
  rs485_serial.begin( 9600 );
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

void loop()
{
  // mirror USB serial in to rs485
  while( Serial.available() > 1 )
  {
    int count = Serial.available();
    byte data[count+1];
    for ( int i=0; i<count; i++ )
    {
      data[i] = Serial.read();
      // echo back with a _ before (to show we sent it)
      Serial.print( "_" );
      Serial.print( (byte)data[i], HEX );
    }
    rs485Write( data, count );
  }
  // mirror rs485 serial out to USB
  while( rs485_serial.available() > 0 )
  {
    Serial.print(" ");
    Serial.print( (byte)rs485_serial.read(), HEX );
  }
}

