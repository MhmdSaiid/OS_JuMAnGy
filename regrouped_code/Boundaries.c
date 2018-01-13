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
#include "scouting.h"
extern float x_position;
extern float y_position;
extern boundary_t ** boundaries;


void bound(void )
{
/* written by Justine Deloménie(the else,update map, comments), Gautier Dervaux(stopping criterion and review) and Mohammed Saeed(squeleton of the code)*/
float initial_position_x = x_position;
float initial_position_y = y_position;

float start_boundary_x = x_position;
float start_boundary_y = y_position;
float end_boundary_x;
float end_boundary_y;
//int compt=0;
float angle = 90.0;
 	while(1){
		while(read_US()>100){ //while we don't meet an obstacle that is to say when we dont reach the next boundary
			move(SPEED_LINEAR,1000,0,'F');
      			end_boundary_x=x_position;
			end_boundary_y=y_position;
			add_bound_line(&boundaries,start_boundary_x,start_boundary_y,end_boundary_x,end_boundary_y); //update the map with the positions of the boundary
			rotate_car(90.0,'R',SPEED_CIRCULAR);
			if(read_US()<200){ //if the boundary is still there
				rotate_car(angle,'L',SPEED_CIRCULAR);
				start_boundary_x=x_position;
				start_boundary_y=y_position;

			}
			else {

				//rotate_car(10.0, 'R',SPEED_CIRCULAR);
				int smtg = move(SPEED_LINEAR,1700,0,'F');
				if(smtg==0){ // if boundary not found
					rotate_car(60, 'R', SPEED_CIRCULAR);
					move(SPEED_LINEAR, 0,1,'F');
				}
				printf("smtg : %d", smtg);
				find_right_angle_obst();
				rotate_car(angle, 'L', SPEED_CIRCULAR);
				start_boundary_x=x_position;
				start_boundary_y=y_position;

				break; 
			}
			Sleep(150);
		
		}
		rotate_car(angle,'L',SPEED_CIRCULAR);
		start_boundary_x = x_position;
		start_boundary_y = y_position;
		if( (abs(x_position-initial_position_x)<20) && (abs(y_position-initial_position_y)<20) ) { //returned to initial position 
			break;
		}
	}
}
