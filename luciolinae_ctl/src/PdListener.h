//
//  PdListener.h
//  luciolinae_ctl
//
//  Created by damian on 9/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"
#include "ofxPd.h"

class PdListener: public ofxPdListener
{
public:
	/// print
	void printReceived(const std::string& message) { ofLog(OF_LOG_NOTICE, "pd: %s", message.c_str() ); };
	
	
};

