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


# turn on auto-flush
$|++;
while(<stdin>) {

	$port->write($_);
    
}

