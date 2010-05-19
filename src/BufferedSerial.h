/*
 *  BufferedSerial.h
 *  serialExample
 *
 *  Created by damian on 07/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"

extern void delayUs( unsigned long us );

class BufferedSerial
{
public:
	
	virtual void setup( ofSerial* _serial );
	virtual void shutdown();
	virtual void beginWrite();
	// blocks until all bytes are written + return true. returns false on error
	virtual bool writeBytes( unsigned char* buffer, int count );
	virtual void endWrite();
	
	
protected:
	int sent_this_block;

	ofSerial* serial;
	
};
