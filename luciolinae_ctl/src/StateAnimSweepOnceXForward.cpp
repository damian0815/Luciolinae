#include "StateAnimSweepOnceXForward.h"

const char* StateAnimSweepOnceXForward::NAME="sweep_once_x_forward";

void StateAnimSweepOnceXForward::enter()
{
	StateAnimSweepOnceX::enter();
	reverse = false;
}


