#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "math.h"
#include "ev3.h"
#include "ev3_port.h"
#include "ev3_sensor.h"
#include "ev3_tacho.h"
// WIN32 /////////////////////////////////////////
#ifdef __WIN32__

#include <windows.h>

// UNIX //////////////////////////////////////////
#else
#include <unistd.h>
#define Sleep( msec ) usleep(( msec ) * 1000 )

//////////////////////////////////////////////////
#endif
#ifndef DETECT_FUNC
#define DETECT_FUNC
#include "detect_disting_functions.h"
#endif
#ifndef MOVE_FUNC
#define MOVE_FUNC
#include "moveFunctions.h"
#endif
#ifndef READ_FUNC
#define READ_FUNC
#include "readFunctions.h"
#endif

#ifndef INITIALIZE
#define INITIALIZE
#include "defines.h"
#endif

#ifndef BT
#define BT
#include "bluetoothFunctions.h"
#endif

#ifndef POS_FUNC
#define POS_FUNC
#include "positionFunctions.h"
#endif

#ifndef THREADSEND
#define THREADSEND
#include "sendPositionThread.h"
#endif

#ifndef SCOUT
#define SCOUT
#include "scouting.h"
#endif
#ifndef LINKED
#define LINKED
#include "linkedlist.h"
#endif
#ifndef BOUND
#define BOUND
#include "Boundaries.h"
#endif

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

extern float x_position;
extern float y_position;
extern float relative_angle;
extern float velocity;
extern int COLOR_VAL;
extern float INTENSITY_VAL;
extern bool TOUCHING;
extern float US_VAL;
extern float ANG_VAL;

extern uint8_t * map;
void limitObst(int obstacleType){
/*
this function is supposed to be used after detecting an obstacle during scouting.
it will turn around the obstacle then avoid it and put the robot towards the initial direction
but after the obstacle in the same scouting line drawn
*/
	int end=0;
	int distance = 5; //in cm
	float x_init = x_position;
	float y_init = y_position;
	float xDecal;
	float yDecal;
	float xDecal2;
	float yDecal2;
	//int side=1;
	int time = (int)(distance/velocity*1000);
	char directionsGoingRight[2] = {'R','L'};
	uint8_t directionIndex;
	if (relative_angle<10) directionIndex = 1; //going right
	if ((relative_angle>170) && (relative_angle<190)) directionIndex = 0; //going left
	bool besideObstacle = true;
	int angleTracker = relative_angle;
	rotate_car(90, directionsGoingRight[directionIndex], SPEED_CIRCULAR);
	int obstacleWhileMoving;
	uint8_t side=1;
	while(1){ //Repeat routine of going alongside the obstacle and checking if still here and
		move(SPEED_LINEAR,time,0,'F');
		rotate_car(90, directionsGoingRight[(directionIndex+1)%2], SPEED_CIRCULAR);
		US_VAL = read_US();
		printf("y_position= %f\n",y_position);
		if (abs(relative_angle-angleTracker)==180 && y_position== (y_init) ){
			rotate_car(180, directionsGoingRight[directionIndex], SPEED_CIRCULAR);
			break;
		}
		if (!detect_obstacle()){
				printf("No Obstacle detected Us = %f\n",US_VAL);
				rotate_car(90, directionsGoingRight[directionIndex], SPEED_CIRCULAR);
				obstacleWhileMoving = move(SPEED_LINEAR,floor(time),0,'F');
				rotate_car(90, directionsGoingRight[(directionIndex+1)%2], SPEED_CIRCULAR); //get some distance from the obstacle
				obstacleWhileMoving = move(SPEED_LINEAR,floor(time*1.5),0,'F');
				if(obstacleWhileMoving){
					//find_right_angle_obst();
					setOnMap(map, x_position, y_position, obstacleType);
					rotate_car(90, directionsGoingRight[directionIndex], SPEED_CIRCULAR);
				}
				else{
					setOnMap(map, x_position, y_position, obstacleType);
					if (side==3){
						printf("DONE\n");
						//going back to end of obstacle
						rotate_car(180, directionsGoingRight[(directionIndex+1)%2], SPEED_CIRCULAR);
						move(SPEED_LINEAR,floor(time*3),0,'F');
						rotate_car(90, directionsGoingRight[directionIndex], SPEED_CIRCULAR);
						move(SPEED_LINEAR,floor(time*3),0,'F');
						rotate_car(90, directionsGoingRight[(directionIndex+1)%2], SPEED_CIRCULAR);
						return;
					}
					else side++;
				}
		}
		else{
			setOnMap(map,x_position, y_position, obstacleType);
			printf("Obstacle is still here \n");
			rotate_car(90, directionsGoingRight[directionIndex], SPEED_CIRCULAR);
		}
	}
}

int checkBoundary(int x,int y){
	printf("x = %d, y = %d\n",x,y);
	int x_dimension = 4;
	int y_dimension = 4;
	if(check_area_obstacle(map, x, y, x_dimension, y_dimension, BOUNDARIES)){

		return 1;
	}
	return 0;

}

void scouting(){
	int distance = 5; //in cm
	int time = (int)(distance/velocity*1000);
	int obst;
	int finished=0;
	int boundaryMet=0;
	int goingRight=1;
	float former_x = x_position;
	float former_y = y_position;
	float new_x;
	float new_y;
	char directionsGoingRight[2] = {'R','L'};
	while(!finished){
		boundaryMet=0;
		former_x=x_position;
		former_y=y_position;
		move(SPEED_LINEAR, 0, 1, 'F');
		new_x=x_position;
		new_y=y_position;
		add_line_of(map, (int)(floor(former_x/5)), (int)(floor(former_y/5)), (int)(floor(new_x/5)), (int)(floor(new_y/5)), EMPTY);
		//stops when there is an obstacle or a boundary
		printf("US_VAL = %f\n",US_VAL);
		if(checkBoundary( (US_VAL/10*cos(relative_angle)+x_position), (y_position +(US_VAL/10*sin(relative_angle)) )) ){
			//if it is a boundary according to our array
			printf("Bounary met\n");
			print_map(map);
			move(SPEED_LINEAR, time/2, 0, 'B'); //goes back a little in order to have enough place to rotate
			rotate_car(90,directionsGoingRight[goingRight], SPEED_CIRCULAR);
			former_x=x_position;
			former_y=y_position;
			move(SPEED_LINEAR,time, 0, 'F');
			new_x=x_position;
			new_y=y_position;
			add_line_of(map, (int)(floor(former_x/5)), (int)(floor(former_y/5)), (int)(floor(new_x/5)), (int)(floor(new_y/5)), EMPTY);
			rotate_car(90,directionsGoingRight[goingRight], SPEED_CIRCULAR);
			goingRight=(goingRight+1)%2;
		}
		else {
			obst=distinguish_obstacle();
			setOnMap(map, x_position, y_position, obst);
			printf("obstacle of type : %d", obst);
			limitObst(obst);
			finished=true;
			return;
		}
	}
}
