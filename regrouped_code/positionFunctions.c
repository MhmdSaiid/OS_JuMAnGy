#include <stdlib.h>
#include <stdio.h>
extern float velocity;
extern float val;
extern float x_position,y_position;
extern float relative_angle;

#include "positionFunctions.h"
#include <math.h>

void update_position(int time){
	float distance= time*velocity/1000;
	x_position += distance*cos(relative_angle*val);
	y_position += distance*sin(relative_angle*val);
}
