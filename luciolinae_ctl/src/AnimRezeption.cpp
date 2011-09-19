//
//  AnimRezeption.cpp
//  luciolinae_ctl
//
//  Created by damian on 9/18/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "AnimRezeption.h"
#include "Osc.h"
#include "ofxVectorMath.h"

const char* AnimRezeption::NAME = "rezeption";

AnimRezeption::AnimRezeption( Lights* _lights )
: Animation( _lights )
{
	state = AR_SLEEP;
	timer = 0.0f; // will immediately switch to IN state on first update
	counter = 0;
	random_saved = 1.0f;
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
			sprinkle_timer -= elapsed;
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
				timer = ofRandom( 5, 20 );
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
				timer = ofRandom( 5,20 );
				duration = timer;
				sprinkle_timer = 0.0f;
				break;
			case AR_OUT:
				state = AR_SLEEP;
				//timer = ofRandom( 10, 30 );
				timer = ofRandom( 30, 45 );
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
			if ( counter == 0 )
			{
				pulse_brightness = ofRandomuf();
				pulse_brightness *= pulse_brightness;
				pulse_brightness = 0.5f+pulse_brightness*0.5f;
			}
			
			float brightness = pulse_brightness*ofRandom(0.8f,1.2f);
			float radius = 1.0f/6.0f;
			ofxVec2f pulse_pos[3] = { ofxVec2f( radius, 1.0f/6.0f ), ofxVec2f( radius, 3.0f/6.0f ), ofxVec2f( radius, 5.0f/6.0f ) };
			lights->illuminateCircularArea( pulse_pos[counter].x, pulse_pos[counter].y, radius, true, brightness );
			
			ofxOscMessage m;
			m.setAddress( "/modal-pings/single" );
			m.addFloatArg( brightness*brightness );
			Osc::getInstance()->sendMessage( m );
		}
		else
		{
			// needed to pass on the chord
			ofxOscMessage m;
			m.setAddress( "/modal-pings/single" );
			m.addFloatArg( 0 );
			Osc::getInstance()->sendMessage( m );
			/*ofxOscMessage m;
			m.setAddress( "/modal-pings/chord" );
			Osc::getInstance()->sendMessage( m );*/
		}
		
		
		counter++;
		// if we should go again
		if ( counter == 0 )
		{
			random_saved = ofRandomuf();
			random_saved = random_saved*random_saved*3.0f + 1.0f;
		}
		if ( counter < 4 )
			timer = random_saved;
		
/*		
		// testing purposes: never leave this state
		if ( counter >= 3 )
		{
			timer = random_saved;
			counter = 0;
		}*/

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
	

	// pulse a sprinkly
	while ( sprinkle_timer < 0 )
	{
		bool found = false;
		float search_x = ofRandom( 0, offset );
		for ( int i=0; i<lights->getNumLights(); i++ )
		{
			int which = ofRandom(0, lights->getNumLights()*0.999f);
			const Light& l = lights->getLight(which);
			if ( fabsf(l.getX() - search_x) < offset*0.3f )
			{
				found = true;
				float brightness = (1.0f-percent)*ofRandom( 0.5f, 1.0f);
				lights->pulse( which, brightness );
				ofxOscMessage m;
				m.setAddress( "/sprinkles-pitched/ping" );
				m.addFloatArg( l.getX()/offset );
				m.addFloatArg( brightness*brightness );
				Osc::getInstance()->sendMessage( m );
			}
			if ( found )
				break;
		}
		float next_timer = ofRandomuf();
		sprinkle_timer += next_timer*next_timer*0.2f + 0.05f;
	}	
	
	

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



