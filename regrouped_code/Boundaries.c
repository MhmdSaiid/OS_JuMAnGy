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
extern float relative_angle;
extern float velocity;
extern float val;
extern boundary_t ** boundaries;
extern float US_VAL;
extern uint8_t * map;

void bound(void )
{
/* written by Justine Deloménie(the else,update map, comments), Gautier Dervaux(stopping criterion and review) and Mohammed Saeed(squeleton of the code)*/
float former_x = x_position;
float former_y = y_position;
float new_x;
float new_y;

float initial_position_x = x_position;
float initial_position_y = y_position;
float start_boundary_x = 0//x_position;
float start_boundary_y = 0;// at the start  of the routine the boundariie is 0 y_position;
float end_boundary_x;
float end_boundary_y;
float angle = 90.0;
int distance = 10;
int timeToStop = (int)(distance/velocity*1000);
//the first boundary may not be physical
int goingRight=1;

move(SPEED_LINEAR,0,1,'F');
end_boundary_x=(US_VAL/10*cos(relative_angle*val)+x_position);
end_boundary_y=0;
add_bound_line(&boundaries,start_boundary_x,start_boundary_y,end_boundary_x,end_boundary_y); //update the map with the positions of the boundary
//keep track robot position

new_x=x_position;
new_y=y_position;
printf("add line: %f, %f, into %f, %f", former_x, former_y, new_x, new_y);
add_big_line_of(map, (int)(floor(former_x)), (int)(floor(former_y)), (int)(floor(new_x)), (int)(floor(new_y)),2, EMPTY);/*update the map with the places we explored without meeting an obstacle*/

rotate_car(angle,'L',SPEED_CIRCULAR);
start_boundary_x = end_boundary_x;
start_boundary_y = end_boundary_y;
move(SPEED_LINEAR,timeToStop*2,0,'F');
end_boundary_y = y_position;
add_bound_line(&boundaries,start_boundary_x,start_boundary_y,end_boundary_x,end_boundary_y);
start_boundary_x = end_boundary_x;
start_boundary_y = end_boundary_y;
	while(1){
		while(read_US()>100){ //while we don't meet an obstacle that is to say when we dont reach the next boundary
			former_x=x_position;
			former_y=y_position;
      if( abs(y_position-initial_position_y)<20 ) { //returned to initial position
        if(relative_angle<0) rotate_car(relative_angle,'L',SPEED_CIRCULAR);
        else rotate_car(relative_angle,'R',SPEED_CIRCULAR);
	return;
  		}
      start_boundary_x=(US_VAL/10*cos((relative_angle+90.0)*val)+x_position); // We are to the right of the boundary
      start_boundary_y=(y_position +(US_VAL/10*sin((relative_angle+90.0)*val)));
			move(SPEED_LINEAR,1000,0,'F');

			new_x=x_position;
			new_y=y_position;
      printf("add line: %f, %f, into %f, %f", former_x, former_y, new_x, new_y);
			add_big_line_of(map, (int)(floor(former_x)), (int)(floor(former_y)), (int)(floor(new_x)), (int)(floor(new_y)),2, EMPTY);/*update the map with the places we explored without meeting an obstacle*/

      rotate_car(90.0,'R',SPEED_CIRCULAR);
			if(read_US()<200){ //if the boundary is still there
        US_VAL = read_US();
        end_boundary_x=(US_VAL/10*cos(relative_angle*val)+x_position);//The boundary is definetly still here so we draw it form the start to the end
        end_boundary_y=(y_position +(US_VAL/10*sin(relative_angle*val)));
        start_boundary_x = end_boundary_x;
        start_boundary_y = end_boundary_y;
        rotate_car(angle,'L',SPEED_CIRCULAR);
			}
			else {
				//rotate_car(10.0, 'R',SPEED_CIRCULAR);
			  former_x=x_position;
			  former_y=y_position;

				int smtg = move(SPEED_LINEAR,1700,0,'F');

        new_x=x_position;
        new_y=y_position;
        printf("add line: %f, %f, into %f, %f", former_x, former_y, new_x, new_y);
        add_big_line_of(map, (int)(floor(former_x)), (int)(floor(former_y)), (int)(floor(new_x)), (int)(floor(new_y)),2, EMPTY);/*update the map with the places we explored without meeting an obstacle*/


				if(smtg==0){ // if boundary not found
		      rotate_car(60, 'R', SPEED_CIRCULAR);

        	former_x=x_position;
        	former_y=y_position;
		      move(SPEED_LINEAR, 0, 1,'F');
	        new_x=x_position;
	        new_y=y_position;
    			printf("add line: %f, %f, into %f, %f", former_x, former_y, new_x, new_y);
	        add_big_line_of(map, (int)(floor(former_x)), (int)(floor(former_y)), (int)(floor(new_x)), (int)(floor(new_y)),10, EMPTY);/*update the map with the places we explored without meeting an obstacle*/
				}
				find_right_angle_obst();
        end_boundary_x=(US_VAL/10*cos(relative_angle*val)+x_position);
        end_boundary_y=(y_position +(US_VAL/10*sin(relative_angle*val)));
        add_bound_line(&boundaries,start_boundary_x,start_boundary_y,end_boundary_x,end_boundary_y); //update the map with the positions of the boundary
        start_boundary_x=end_boundary_x;
        start_boundary_y=end_boundary_y;
				rotate_car(angle, 'L', SPEED_CIRCULAR);
			}
		Sleep(150);
		US_VAL = read_US();
		printf("US_VAL = %f\n",US_VAL);
		}
    start_boundary_x=(US_VAL/10*cos(relative_angle*val)+x_position); //We are at an angle so the next boundary is in front
    start_boundary_y=(y_position +(US_VAL/10*sin(relative_angle*val)));
		rotate_car(angle,'R',SPEED_CIRCULAR);//get awaay from the corner so turn to the charted obstacle move back then 180
		move(SPEED_LINEAR,timeToStop*2,0,'B');
		rotate_car(180.0,'L',SPEED_CIRCULAR);
    print_map(map);
	}
}
