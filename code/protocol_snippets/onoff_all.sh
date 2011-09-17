#!/bin/bash
while (true); 
do 
    echo off; 
    cat turn_all_off.hex | ./sendserial.pl _dev 9600
    usleep 1000; 
    echo on; 
    cat turn_all_on.hex | ./sendserial.pl _dev 9600
    usleep 500; 
done
