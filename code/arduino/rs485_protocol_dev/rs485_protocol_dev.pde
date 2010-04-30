

// the led pin
static const int LED_PIN = 5;
static unsigned char RS485_TRANSMIT_ENABLE_PIN = 2;

// message is 0x01 <led value>
static unsigned char FUNC_SET_LED_VAL = 0x01;

// the buffer for messages
unsigned char msg_buf[2];

void setup()
{
//  pinMode( LED_PIN, OUTPUT );
  // turn off transmit on the RS485 initially
  pinMode( RS485_TRANSMIT_ENABLE_PIN, OUTPUT );
  digitalWrite( RS485_TRANSMIT_ENABLE_PIN, LOW );

  Serial.begin(19200); 
  msg_buf[0] = FUNC_SET_LED_VAL;
}

void setAndSend( unsigned char level )
{
  msg_buf[1] = level;
  // enable RS485 write
  digitalWrite( RS485_TRANSMIT_ENABLE_PIN, HIGH );
  Serial.write( msg_buf, 2 );
  delay( 4 );
  // RS485 done
  digitalWrite( RS485_TRANSMIT_ENABLE_PIN, LOW );
  // set our LED level
  analogWrite( LED_PIN, level );
}

void loop()
{
  for ( int val=0; val<256; val ++ )
  {
    setAndSend( val );
    delayMicroseconds( 10 );
  }
  for ( int val=255; val>=0; val -- )
  {
    setAndSend( val );
    delayMicroseconds( 10 );
  }
}


