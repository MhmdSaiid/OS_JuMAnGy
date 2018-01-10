#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "math.h"
#include "ev3.h"
#include "ev3_port.h"
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
#ifndef POS_FUNC
#define POS_FUNC
#include "positionFunctions.h"
#endif
#include "scouting.h"
//#include "linkedlist.h"


#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

extern float x_position;
extern float y_position;
extern int relative_angle;
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
	int time = (int)(distance/velocity*3000);
	char directionsGoingRight[2] = {'R','L'};
	uint8_t directionIndex;
	if (relative_angle<10) directionIndex = 1; //goig right
	if ((relative_angle>170) && (relative_angle<190)) directionIndex = 0; //goig left
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
				obstacleWhileMoving = move(SPEED_LINEAR,floor(time*1.5),0,'F');
				rotate_car(90, directionsGoingRight[(directionIndex+1)%2], SPEED_CIRCULAR); //get some distance from the obstacle
				obstacleWhileMoving = move(SPEED_LINEAR,floor(time*1.5),0,'F');
				if(obstacleWhileMoving){
					rotate_car(90, directionsGoingRight[directionIndex], SPEED_CIRCULAR);
				}
				else{
					if (side==3){
						printf("DONE\n");
						//going back to end of obstacle
						rotate_car(180, directionsGoingRight[(directionIndex+1)%2], SPEED_CIRCULAR);
						move(SPEED_LINEAR,floor(time*1.5),0,'F');
						rotate_car(90, directionsGoingRight[directionIndex], SPEED_CIRCULAR);
						move(SPEED_LINEAR,floor(time*1.5),0,'F');
						rotate_car(90, directionsGoingRight[(directionIndex+1)%2], SPEED_CIRCULAR);
						break;
					}
					else side++;
				}
		}
		else{
			printf("Obstacle is still here \n");
			rotate_car(90, directionsGoingRight[directionIndex], SPEED_CIRCULAR);
		}
	}
/*


	for(side=1;side<=5; side++){ //5 in order to do the missing part of the first side at the end
		end=0;
		printf("side : %d", side);
		while(end==0){

			move(SPEED_LINEAR,time,0,'F');
			//printf("d :%d, velocity : %f, time : %f", distance, velocity, distance/velocity*1000);
			//move(SPEED_LINEAR,2000,0,'F');
			rotate_car(90, 'L', SPEED_CIRCULAR);
			if(!detect_obstacle()){
			//if there is no obstacle we reached the limit of this side
				end=1;
				if(side==1){
					xDecal=abs(x_position-x_init);
					yDecal=abs(y_position-y_init);
					x_init=x_position;
					y_init = y_position;
//to save our relative position to the obstacle when we arrived to be able to continue moving straight the same line as we were following before encountering the obstacle
				}
				if (side==2){
					xDecal2=abs(x_position-x_init);
					yDecal2=abs(y_position-y_init);
// to save the lenght of the seconde size in order to go after that at the end of the function
				}
			}
			else{
				//addToMap(obstacleType);
				rotate_car(90, 'R', SPEED_CIRCULAR);
			}


		}
	}/*
	move(SPEED_LINEAR, max(xDecal,yDecal)/velocity*1000, 0, 'F'); //longueur //go after the obstacle
	rotate_car(90, 'L', SPEED_CIRCULAR);
	move(SPEED_LINEAR, max(xDecal,yDecal)/velocity*1000,0, 'F'); //largeur //realignment with initial position
	rotate_car(90, 'R', SPEED_CIRCULAR);*/
}
/*
int checkBoundary(int x,int y){
	uint8_t valueFromMap=getFromMap(map,x,y);
	if(valueFromMap == BOUNDARIES){
		return 1;
	}
	return 0;

}
*/
void scouting(){
	int distance = 5; //in cm
	int time = (int)(distance/velocity*4000);
	int obst;
	int finished=0;
	int boundaryMet=0;
	int goingRight=1;
	while(!finished){
		boundaryMet=0;
		while(!boundaryMet){
			move(SPEED_LINEAR, 0, 1, 'F');
			//stops when there is an obstacle or a boundary
			//if(checkBoundary(floor(x_position/5),floor(y_position/5))) {
			if(false){ //The map is not definied yet
				//if it is a boundary according to our linked list
				if(goingRight){
					move(SPEED_LINEAR, time/2, 0, 'B'); //goes back a little in order to have enough place to rotate
					rotate_car(90,'L', SPEED_CIRCULAR);
					move(SPEED_LINEAR,time, 0, 'F');
					rotate_car(90,'L', SPEED_CIRCULAR);
					goingRight=0;
				}
				else if(!goingRight){
					move(SPEED_LINEAR, time/2, 0, 'B');
					rotate_car(90,'R', SPEED_CIRCULAR);
					move(SPEED_LINEAR,time, 0, 'F');
					rotate_car(90,'R', SPEED_CIRCULAR);
					goingRight=1;
				}
				boundaryMet=1;
			}
			else {
				//obst=distinguish_obstacle();
				limitObst(obst);
				finished=true;
			}
		}
	}

}
