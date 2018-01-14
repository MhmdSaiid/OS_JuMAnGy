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
float start_boundary_x = x_position;
float start_boundary_y = y_position;
float end_boundary_x;
float end_boundary_y;
float angle = 90.0;
int distance = 10;
int timeToStop = (int)(distance/velocity*1000);
//the first boundary may not be physical
int goingRight=1;


move(SPEED_LINEAR,0,1,'F');
end_boundary_x=x_position;
end_boundary_y=y_position;
add_bound_line(&boundaries,start_boundary_x,start_boundary_y,end_boundary_x,end_boundary_y); //update the map with the positions of the boundary
//keep track robot position
new_x=x_position;
new_y=y_position;
printf("add line: %f, %f, into %f, %f", former_x, former_y, new_x, new_y);
add_big_line_of(map, (int)(floor(former_x)), (int)(floor(former_y)), (int)(floor(new_x)), (int)(floor(new_y)),2, EMPTY);/*update the map with the places we explored without meeting an obstacle*/
rotate_car(angle,'L',SPEED_CIRCULAR);
start_boundary_x = x_position;
start_boundary_y = y_position;

 	while(1){
		while(read_US()>100){ //while we don't meet an obstacle that is to say when we dont reach the next boundary
      former_x=x_position;
      former_y=y_position;
      move(SPEED_LINEAR,1000,0,'F');
      new_x=x_position;
      new_y=y_position;
      printf("add line: %f, %f, into %f, %f", former_x, former_y, new_x, new_y);
      add_big_line_of(map, (int)(floor(former_x)), (int)(floor(former_y)), (int)(floor(new_x)), (int)(floor(new_y)),2, EMPTY);/*update the map with the places we explored without meeting an obstacle*/
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
        former_x=x_position;
        former_y=y_position;

				int smtg = move(SPEED_LINEAR,1700,0,'F');

        new_x=x_position;
        new_y=y_position;
        printf("add line: %f, %f, into %f, %f", former_x, former_y, new_x, new_y);
        add_big_line_of(map, (int)(floor(former_x)), (int)(floor(former_y)), (int)(floor(new_x)), (int)(floor(new_y)),2, EMPTY);/*update the map with the places we explored without meeting an obstacle*/

        end_boundary_x=x_position;
        end_boundary_y=y_position;
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
				printf("smtg : %d\n", smtg);
				find_right_angle_obst();
				printf("end right angle\n");
				rotate_car(angle, 'L', SPEED_CIRCULAR);
				printf("end rotateCar\n");
				start_boundary_x=x_position;
				start_boundary_y=y_position;

			}
		Sleep(150);
		US_VAL = read_US();
		printf("US_VAL = %f\n",US_VAL);
		}
		rotate_car(angle,'R',SPEED_CIRCULAR);//get awaay from the corner
		move(SPEED_LINEAR,timeToStop*2,0,'B');
		rotate_car(180.0,'L',SPEED_CIRCULAR);
		start_boundary_x = x_position;
		start_boundary_y = y_position;
    print_map(map);
		if( (abs(x_position-initial_position_x)<20) && (abs(y_position-initial_position_y)<20) ) { //returned to initial position
			break;
		}
	}
}
