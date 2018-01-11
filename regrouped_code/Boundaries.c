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

extern float x_position;
extern float y_position;
extern boundary_t ** boundaries;


void bound(void )
{
float start_x = x_position;
float start_y=y_position;
float new_x;
float new_y;
int compt=0;

 	while(1){
		while(read_US()>100){ //while we don't meet an obstacle that is to say when we dont reach the next boundary
			move(SPEED_LINEAR,1000,0,'F');
			new_x=x_position;
			new_y=y_position;
			add_bound_line(&boundaries,start_x,start_y,new_x,new_y);
			rotate_car(90,'R',SPEED_CIRCULAR);
			//Sleep(500);
			if(read_US()<200){ //if the boundary is still there
				rotate_car(90,'L',SPEED_CIRCULAR);
				start_x=x_position;
				start_y=y_position;

			}
			else {
				start_x=x_position;
				start_y=y_position;
				break; //maybe turn right ?
			}
			Sleep(150);
			cont+=1
		}
		rotate_car(90,'L',SPEED_CIRCULAR);
		start_x=x_position;
		start_y=y_position;
		if((x_position>80)&&(x_position<100) && (y_position>10)&&(y_position<30) || compt=3) //returned to initial position ?
		break;
	}
}
