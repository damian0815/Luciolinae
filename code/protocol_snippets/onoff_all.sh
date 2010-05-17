#!/bin/bash
while (true); 
do 
    echo off; 
    cat turn_all_off.hex | ~/bin/sendserial.pl /dev/tty.usbserial-A5001aub 9600; 
    usleep 1000; 
    echo on; 
    cat turn_all_on.hex | ~/bin/sendserial.pl /dev/tty.usbserial-A5001aub 9600; 
    usleep 500; 
done
