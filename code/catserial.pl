#!/usr/bin/perl
# Sample Perl script to transmit number
# to Arduino then listen for the Arduino
# to echo it back

use Device::SerialPort;

# Set up the serial port
# 19200, 81N on the USB ftdi driver
my $port = Device::SerialPort->new($ARGV[0]);
$port->databits(8);
$port->baudrate($ARGV[1]);
$port->parity("none");
$port->stopbits(1);


#while ($count<16) {
my $count = 0;
# turn on auto-flush
$|++;
while(1) {
    # Poll to see if any data is coming in
    my ($count_in, $string_in) = $port->read(1);
	if ( $count_in > 0 )
	{
		print $string_in;
		#
		#for ( my $i=0; $i<length($string_in); $i++ )
		#{
		#	my $char = substr( $string_in, $i, 1 );
		#	printf ("%02x ",  ord($char) );  
		#	if ( ++$count > 8 )
		#	{
		#		print "\n";
		#		$count = 0;
		#	}
		#}
	}
}

