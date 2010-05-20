#!/bin/bash
while (true); 
do 
    echo off; 
    cat turn_1_off.hex | ~/bin/sendserial.pl /dev/tty.usbserial-FTT8R2AA 9600; 
    usleep 1000; 
    echo on; 
    cat turn_1_on.hex | ~/bin/sendserial.pl /dev/tty.usbserial-FTT8R2AA 9600; 
    usleep 20; 
done
