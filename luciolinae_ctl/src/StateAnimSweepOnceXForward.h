/*
 *  StateAnimSweepOnceXForward.h
 *  luciolinae_ctl
 *
 *  Created by damian on 05/06/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "StateAnimSweepOnceX.h"

class StateAnimSweepOnceXForward : public StateAnimSweepOnceX
{
public:
	StateAnimSweepOnceXForward( Lights* _lights ) : StateAnimSweepOnceX( _lights ) {};
	
	static const char* NAME;
	string getName() { return NAME; }
	
	// all work happens here
	void enter();
	
private:
	
};

