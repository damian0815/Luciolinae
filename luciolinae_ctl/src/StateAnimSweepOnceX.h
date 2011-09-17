/*
 *  StateAnimSweepOnceX.h
 *  luciolinae_ctl
 *
 *  Created by damian on 05/06/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "StateAnimation.h"

class StateAnimSweepOnceX : public StateAnimation
{
public:
	StateAnimSweepOnceX( Lights* _lights ) : StateAnimation( _lights ) { speed = 3.0f; };
	virtual ~StateAnimSweepOnceX() {};
	
	static const char* NAME;
	string getName() { return NAME; }
	
	void draw();
	
	// all work happens here
	virtual void enter();
	// immediately exit
	bool isFinished() { return pos > endpoint; }
	
	// update
	void update( float elapsed );
	

	
protected:
	
	float endpoint;

	bool reverse;
	bool finished;
	float pos;
	float speed;
	int sweep_dir;
};
