#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "math.h"
#include "ev3.h"
#include "ev3_port.h"
#include "math.h"
#include "defines.h"
#include "Boundaries.h"
#include "readFunctions.h"
#include "linkedlist.h"
#include "moveFunctions.h"

extern float x_position;
extern float y_position;
extern boundary_t ** boundaries;


void bound(void )
{
float initial_position_x = x_position;
float initial_position_y = y_position;

float start_x = x_position;
float start_y = y_position;
float new_x;
float new_y;
//int compt=0;
float angle = 90.0;
 	while(1){
		while(read_US()>100){ //while we don't meet an obstacle that is to say when we dont reach the next boundary
			move(SPEED_LINEAR,1000,0,'F');
      new_x=x_position;
			new_y=y_position;
			add_bound_line(&boundaries,start_x,start_y,new_x,new_y);
			rotate_car(90.0,'R',SPEED_CIRCULAR);
			//Sleep(500);
			if(read_US()<200){ //if the boundary is still there
				rotate_car(angle,'L',SPEED_CIRCULAR);
				start_x=x_position;
				start_y=y_position;

			}
			else {
				start_x=x_position;
				start_y=y_position;
				break; //maybe turn right ?
			}
			Sleep(150);
			//compt+=1;
		}
		rotate_car(angle,'L',SPEED_CIRCULAR);
		start_x = x_position;
		start_y = y_position;
		if( abs(x_position-initial_position_x)<20 && abs(y_position-initial_position_y)<20) ){ //returned to initial position ?
		//if(compt==3){
			break;
		}
	}
}
