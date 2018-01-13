#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include "ev3.h"
#include "ev3_port.h"
#include "ev3_sensor.h"
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

extern int max_speed;
extern float x_position, y_position;
extern int relative_angle;
extern float velocity;
extern int COLOR_VAL;
extern float INTENSITY_VAL;
extern bool TOUCHED;
extern float US_VAL;
extern float ANG_VAL;
extern bool TOUCHING;
extern const char const *color[];
extern uint8_t motor[ 3 ];


int detect_obstacle(void){
	/* written by Justine Deloménie*/
	int dist_thresh = 120; //distance threshold
	if ( US_VAL<= dist_thresh ) //if we are too close from an obstacle according to the ultrasound sensor, then we say there is an obstacle
	{
		return 1;
	}
	if (TOUCHING) //idem if the button touch is activated. It means there is an obstacle but not in front of us
	{
		return 2;
	}
	if (US_VAL > dist_thresh) //the next obstacle is still far we can continue to move
	{
		return 0;
	}
	return 1;
}

void find_right_angle_obst(){
	/*written first by Justine Deloménie, improved by Gautier Dervaux*/
	/* I didnt take into account the fact that a second obstacle could touch the first one and so become a problem , I think if it is turning towards the second one it will stop in front of this one instead of the first one*/
	uint8_t i = 0;
	int distance = 3; //in cm
	int timeToStop = (int)(distance/velocity*1000);
	move(SPEED_LINEAR,timeToStop,0,'B');
	calibrate_gyro();
	ANG_VAL=0;
	Sleep(1000);
	US_VAL = read_US();
	US_VAL = read_US();
	US_VAL = read_US();
	float dist_init_obj = US_VAL;
	float thresh_dist_ang = 10;
	int speed_circular = (int) (SPEED_CIRCULAR*3/2);
	int speed_linear = SPEED_LINEAR;
	int bool_right=0 ; // boolean 1 is right 0 is left
	char rotationDirection[2]={'L','R'};

	int time = 1000;
	rotate_car(10,'R', speed_circular); //we rotate a little to the right
	Sleep(100);
	US_VAL = read_US();
	printf("\n dist_init= %f, value US = %f\n",dist_init_obj, US_VAL);
	if (dist_init_obj>US_VAL) { //if we get closer to the obstacle
		    bool_right = 1; // then we chose the right direction to turn
	}
	if(US_VAL>300){ //if we are far from reaching the angle we want we can turn faster
		float angle = 30/200*US_VAL +(20-300*30/200);
		rotate_car(angle,rotationDirection[bool_right],speed_circular);
		Sleep(1000);
	}
	//To improve next step first roation depending on new reading
	US_VAL=read_US();
	bool_right = (bool_right + 1 + (US_VAL<dist_init_obj) )%2;
	printf("bool %d\n", bool_right);
	float current_dist = US_VAL;
	float previous_dist = current_dist+1;
	/*then turns slower*/
	if(bool_right==1) {
			run_forever(speed_circular,-speed_circular);
	}
	else{
			run_forever(-speed_circular,speed_circular);
	}
	uint8_t sameDist; //To keep track how many times the same dist
	while (previous_dist >= current_dist && current_dist > 35) { //we stop when the value did not change fast during the last small rotations
		if(previous_dist==current_dist) sameDist++;
		else sameDist=0;
		if(sameDist==5){
			break;
		}
		previous_dist = current_dist;
		US_VAL = read_US();
		current_dist = US_VAL;
		printf("previous_dist = %f,current_dist =%f\n",previous_dist,current_dist);

		//printf("previous %f, new %f \n", previous_dist, current_dist);
	}
	stop_car();
	//we can have finished but may be not if we are too close of one corner of the object
	//so we check if its ok or if it is the not lucky case
	/*sleep(10);
	if(bool_right==1){
	    //move left
		printf("check translation left\n\n");
	    rotate_car2(90,'L', speed_circular);
		sleep(1);
	    int bool_move=move(speed, time, 0,'F');
		sleep(1);
	    rotate_car2(90,'R', speed_circular);
		printf("end translation corner\n\n");
	}
	else {
	    //move right
		printf("check translation right\n\n");
	    rotate_car2(90,'R', speed_circular);
		sleep(1);
	    int bool_move=move(speed, time, 0,'F');
		sleep(1);
	    rotate_car2(90,'L', speed_circular);
		printf("end translation corner\n\n");
	}
	update_sensors_value();
	US_VAL=read_US();
	if (current_dist + thresh_dist_ang < US_VAL){
		    //then we were not at the right angle, try again
		    find_right_angle_obst();
	}*/
	ANG_VAL = read_ang();
	relative_angle+=ANG_VAL;
	return;
}

int find_obst_size(int max_size, int count){
	/* written by Justine Deloménie */
	/* I suppose that we just want to know if the size is higher than a limit to distinguish boundaries from unmovable objects */

	int thresh = 20;
	int speed_circular=SPEED_CIRCULAR;
	int speed_linear = SPEED_LINEAR;
	int abs_speed_linear = MAX_SPEED*speed_linear/100;

	float start_dist=US_VAL;

	find_right_angle_obst(); // after that we are at a 90° angle with the obstacle

	rotate_car(90, 'L', speed_circular);

	int d=max_size;//cm
	int v = 25; //cm/s
	int time = 2000; //ms
	printf("\n max_size : %d, speed : %d, d : %d, time : %d \n", max_size, abs_speed_linear, d, time);
	int bool_move = move(speed_linear, time, 0,'F');
	if (bool_move==0) //if there were no additional obstacle that blocked the movement
	{
		ANG_VAL=read_ang();
		rotate_car(90, 'R', speed_circular);
		ANG_VAL=read_ang();
		US_VAL=read_US();
		if (abs(US_VAL-start_dist) < thresh) // if we are still in front of the obstacle
		{
				    /*then it is a boundary because the size is too high for all other types of objects*/
				    /* come back to the original position */
				    rotate_car(90,'R', speed_circular);
				    bool_move = move(speed_linear, time, 0,'F');
				    rotate_car(90,'L', speed_circular);
				    return max_size+1;
		}
		/* else go back to the initial position and returns that it is not long enough to be a boundary*/
		ANG_VAL=read_ang();
		rotate_car(90,'R', speed_circular);
		ANG_VAL=read_ang();
		bool_move = move(speed_linear, time, 0,'F');
		rotate_car(90,'L', speed_circular);
		ANG_VAL=read_ang();
		return 0;

	}
	else{
		//go back
		rotate_car(180,'R', speed_circular);
		bool_move = move(speed_linear, bool_move, 0,'F');
		//try the other side
		bool_move = move(speed_linear, time, 0,'F');
		if(bool_move==0) {
			ANG_VAL=read_ang();
			rotate_car(90, 'L', speed_circular);
			ANG_VAL=read_ang();
			US_VAL=read_US();
			if (abs(US_VAL-start_dist) < thresh) // if we are still in front of the obstacle
			{
					    /*then it is a boundary because the size is too high for all other types of objects*/
					    /* come back to the original position */
					    rotate_car(90,'L', speed_circular);
					    bool_move = move(speed_linear, time, 0,'F');
					    rotate_car(90,'R', speed_circular);
					    return max_size+1;
			}
			//go back and not a boundary cf first try before
			ANG_VAL=read_ang();
			rotate_car(90,'L', speed_circular);
			ANG_VAL=read_ang();
			bool_move = move(speed_linear, time, 0,'F');
			rotate_car(90,'R', speed_circular);
			ANG_VAL=read_ang();
			return 0;

		}
		else {
			//go back and consider it is a boundary
			rotate_car(180,'L', speed_circular);
			bool_move = move(speed_linear, bool_move, 0,'F');
			rotate_car(90,'R', speed_circular);
			return max_size+1;
		}
	}

}

int distinguish_obstacleOld(void)
	/* written by Justine Deloménie. */

{
	int threshold = 50;
	int max_size = 45; //45;
	float old_distance = US_VAL;


	/* for the moment, I consider we are in the small stadium and the obstacles are only those from the teacher */
	int val_color_object = COLOR_VAL;
	update_sensors_value();

	//if the object is red it is unmovable
	if(strcmp(color[val_color_object],"RED")==0)
	{
		printf("movable\n");
		return MOVABLE;
	}

	printf("not red \n");
	sleep(2); //wait for 2 seconds in order to see if the object has moved, if yes, it was a robot
	update_sensors_value();
	float new_distance = US_VAL;
	/*
	if (abs(new_distance-old_distance)>threshold){
		printf("robot\n");
		return ROBOT;
	}
	*/
	int object_size;
	object_size = find_obst_size(max_size,0);
	printf("obj size : %d \n", object_size);
	if (object_size > max_size) //if it is a long object then it is a boundary
	{
		printf("boundaries\n");
		    return BOUNDARIES;
	}
	//otherwise it is an unmovable
	printf("unmovable");
	return UNMOVABLE;
}

int distinguish_obstacle(void)
	/* written by Justine Deloménie */
	//simplified version for all cases, we do not have to identify boundaries because we can check it before thanks to our map
{
	int threshold = 50;
	float old_distance = US_VAL;
	int val_color_object = COLOR_VAL;
	update_sensors_value();

	sleep(2); //wait for 2 seconds in order to see if the object has moved, if yes, it was a robot
	update_sensors_value();
	float new_distance = US_VAL;
	if (abs(new_distance-old_distance)>threshold){
		printf("robot\n");
		return ROBOT;
	}

	if(strcmp(color[val_color_object],"RED")==0) // if it is red
	{
		if(INTENSITY_VAL>=3){ //if it is a high intensity then it is a ball
			printf("movable\n");
			return MOVABLE;
		}
		printf("red but unmovable");
		return UNMOVABLE; //else it must be an object released by an opponent (or us)
	}

	printf("not red \n");
	printf("unmovable");
	return UNMOVABLE; //if is noting before, then it has to be an unmovable one
}
