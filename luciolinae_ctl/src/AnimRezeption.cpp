//
//  AnimRezeption.cpp
//  luciolinae_ctl
//
//  Created by damian on 9/18/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "AnimRezeption.h"
#include "Osc.h"

const char* AnimRezeption::NAME = "rezeption";

AnimRezeption::AnimRezeption( Lights* _lights )
: Animation( _lights )
{
	state = AR_SLEEP;
	timer = 0.0f; // will immediately switch to IN state on first update
	counter = 0;
}

void AnimRezeption::update( float elapsed )
{
	timer -= elapsed;

	
	switch( state )
	{
		case AR_SLEEP:
			break;
			
		case AR_IN:
			updateIn();
			break;
			
		case AR_OUT: 
			updateOut();
			break;
			
		case AR_PULSE:
			updatePulse();
			break;
			
		default:
			break;
	}
	
	if ( timer < 0.0f )
	{
		switch( state )
		{
			case AR_SLEEP:
				state = AR_IN;
				timer = 5.0f;
				duration = timer;
				break;
			case AR_IN:
				state = AR_PULSE;
				timer = 0.0f;
				counter = 0;
				break;
			case AR_PULSE:
				// go to out
				state = AR_OUT;
				timer = 5.0f;
				duration = timer;
				break;
			case AR_OUT:
				state = AR_SLEEP;
				timer = 15.0f;
				break;
			default:
				break;
		}
	}
}


void AnimRezeption::updateIn()
{
	percent = 1.0f-(timer/duration);
	
	// start from the right and go left
	float offset = 1.0f;
	float span = -(1.0f-2.0f/6.0f) + 0.1f;
	
	float x = offset + percent*span;
	float y = 0;
	float dx = 0;
	float dy = 1;
	//printf("illuminating corridor: %f,%f  %f,%f\n", x, y, dx, dy );
	lights->illuminateCorridor( x, y, dx, dy, 1.0f, 0.05f );
	
}

void AnimRezeption::updatePulse()
{
	
	if ( timer < 0.0f )
	{
		if ( counter >= 0 && counter < 3 )
		{
			float radius = 1.0f/6.0f;
			ofVec2f pulse_pos[3] = { ofVec2f( radius, 1.0f/6.0f ), ofVec2f( radius, 3.0f/6.0f ), ofVec2f( radius, 5.0f/6.0f ) };
			lights->illuminateCircularArea( pulse_pos[counter].x, pulse_pos[counter].y, radius );
			
			ofxOscMessage m;
			m.setAddress( "/modal-pings/single" );
			Osc::getInstance()->sendMessage( m );
		}
		else
		{
			ofxOscMessage m;
			m.setAddress( "/modal-pings/chord" );
			Osc::getInstance()->sendMessage( m );
		}
		
		
		counter++;
		// if we should go again
		if ( counter < 4 )
			timer = 1.0f;

	}
	
}

void AnimRezeption::updateOut()
{
	percent = 1.0f-(timer/duration);
	
	// start 1/6 from the left and go off the right
	float offset = 2.0f/6.0f;
	float span = (1.0f-offset)+0.1f;

	float x = offset + percent*span;
	float y = 0;
	float dx = 0;
	float dy = 1;
	//printf("illuminating corridor: %f,%f  %f,%f\n", x, y, dx, dy );
	lights->illuminateCorridor( x, y, dx, dy, 1.0f, 0.05f );

}


void AnimRezeption::draw()
{
	/*
	if ( state == AR_IN || state == AR_OUT )
	{
		// extend illumination over the edges
		float x = percent*1.2f-0.1f;
		float y = 0;
		float dx = 0;
		float dy = 1;
		//printf("illuminating corridor: %f,%f  %f,%f\n", x, y, dx, dy );
		lights->drawIlluminateCorridor( x, y, dx, dy, 0.1f, 0.05f );
	}*/
}



