//
//  AnimRezeption.h
//  luciolinae_ctl
//
//  Created by damian on 9/18/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "Animation.h"

class AnimRezeption: public Animation
{
public:
	AnimRezeption( Lights* _lights );
	
	static const char* NAME;
	
	void update( float elapsed );
	void draw();

protected:

	void updateIn();
	void updatePulse();
	void updateOut();
	
	
	typedef enum 
	{
		AR_SLEEP,
		AR_IN,
		AR_PULSE,
		AR_OUT
	} AnimRezeptionState ;
	
	AnimRezeptionState state;
	
	float timer;
	float duration;
	float percent;
	int counter;
	
};

