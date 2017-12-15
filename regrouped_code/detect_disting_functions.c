#include <stdio.h>                                                                            
#include <stdlib.h>                                                                           
#include <string.h>                                                                           
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
extern int COLOR_VAL;
extern float INTENSITY_VAL;
extern bool TOUCHED;
extern float US_VAL;
extern float ANG_VAL;
extern bool TOUCHING;
extern const char const *color[];
extern uint8_t motor[ 3 ];


int detect_obstacle(void){
	/* written by J.D*/
	int dist_thresh = 120; //in mm
	if (TOUCHING || (US_VAL<= dist_thresh))
	{
		return 1;
	}
	if (US_VAL > dist_thresh)
	{
		return 0;
	}
	return 1;
}

void find_right_angle_obst(){
	/* I didnt take into account the fact that a second obstacle could touch the first one and so become a problem , I think if it is turning towards the second one it will stop in front of this one instead of the first one*/
	update_sensors_value();
	float dist_init_obj = US_VAL;
	float thresh_dist_ang = 10;
	int speed_circular = SPEED_CIRCULAR;
	int speed_linear = SPEED_LINEAR;
	int bool_right=0 ; // boolean 1 is right 0 is left

	int time = 1000;
	rotate_car(10,'R', speed_circular);
	update_sensors_value();
	printf("\n dist_init= %f, value US = %f \n",dist_init_obj, US_VAL); 
	if (dist_init_obj>US_VAL) {
		    bool_right = 1; // we choose the right direction to turn
	}
	//printf("bool %d\n", bool_right);
	float current_dist = US_VAL;
	float previous_dist = current_dist+1;
	while (previous_dist >= current_dist) {
		//while we are not at 90°
		if(bool_right==1) {
				rotate_car(7,'R', speed_circular);
		}
		else{
				rotate_car(7,'L', speed_circular);
		}
		update_sensors_value();
		US_VAL=read_US();
		previous_dist = current_dist;
		current_dist = US_VAL;
		//printf("previous %f, new %f \n", previous_dist, current_dist);
	}
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
	return;
}

int find_obst_size(int max_size, int count){
	/* written by J.D */
	/* I also suppose that we just want to know if the size is higher than a limit to distinguish boundaries from unmovable objects */
	/* finally I consider that it is not possible to meet a second obstacle while trying to define the size of the first one */
	/*movable are only red balls*/

	int thresh = 20;
	int speed_circular=SPEED_CIRCULAR;
	int speed_linear = SPEED_LINEAR;
	int abs_speed_linear = MAX_SPEED*speed_linear/100;

	//int speed=SPEED_LINEAR ; //en cm/s
	//int start_x = x_position;
	//int start_y = y_position;
	float start_dist=US_VAL;

	//float alpha = ANG_VAL;
	find_right_angle_obst(); // after that we are at a 90° angle with the obstacle

	rotate_car(90, 'L', speed_circular);
	//go_to(start_x, start_y+max_size);

	int d=max_size;//cm 
	int v = 25; //cm/s
	int time = 2000; //ms
	printf("\n max_size : %d, speed : %d, d : %d, time : %d \n", max_size, abs_speed_linear, d, time);
	//printf("\ntime :%d \n", time); 
	int bool_move = move(speed_linear, time, 0,'F');
	if (bool_move==0) //if there were no additional obstacle that blocked the movement
	{
		ANG_VAL=read_ang();
		//printf("\nbefor rotating to the right : %f\n", ANG_VAL);
		rotate_car(90, 'R', speed_circular);
		ANG_VAL=read_ang();
		//printf("\nafter rotating to the right : %f\n", ANG_VAL);
		US_VAL=read_US();
		if (abs(US_VAL-start_dist) < thresh) // if we are still in front of the obstacle
		{
				    /*then it is a boundary because the size is too high for all other types of objects*/
				    /* come back to the original position */
				    rotate_car(90,'R', speed_circular);
				    //printf("time to move");
				    bool_move = move(speed_linear, time, 0,'F');
				    rotate_car(90,'L', speed_circular);
				    return max_size+1;
		}
		ANG_VAL=read_ang();
		//printf(" \n\nbefore rotating to the right : %f\n\n", ANG_VAL);
		rotate_car(90,'R', speed_circular);
		ANG_VAL=read_ang();
		//printf(" after rotating to the right : %f\n", ANG_VAL);
		//printf("time to move");
		bool_move = move(speed_linear, time, 0,'F');
		rotate_car(90,'L', speed_circular);
		ANG_VAL=read_ang();
		//printf(" \nafter rotating to the left : %f\n", ANG_VAL);
		return 0;

	}
	else{
		//go back
		rotate_car(180,'R', speed_circular);
		//printf("time to move");
		bool_move = move(speed_linear, bool_move, 0,'F');
		//rotate_car2(90,'L', speed_circular);
		//try the other side
		//rotate_car2(90,'R', speed_circular);
		//printf("time to move");
		bool_move = move(speed_linear, time, 0,'F');
		if(bool_move==0) {
			ANG_VAL=read_ang();
			//printf("\nbefor rotating to the left : %f\n", ANG_VAL);
			rotate_car(90, 'L', speed_circular);
			ANG_VAL=read_ang();
			//printf("\nafter rotating to the left : %f\n", ANG_VAL);
			US_VAL=read_US();
			if (abs(US_VAL-start_dist) < thresh) // if we are still in front of the obstacle
			{
					    /*then it is a boundary because the size is too high for all other types of objects*/
					    /* come back to the original position */
					    rotate_car(90,'L', speed_circular);
					    //printf("time to move");
					    bool_move = move(speed_linear, time, 0,'F');
					    rotate_car(90,'R', speed_circular);
					    return max_size+1;
			}
			ANG_VAL=read_ang();
			//printf(" \n\nbefore rotating to the right : %f\n\n", ANG_VAL);
			rotate_car(90,'L', speed_circular);
			ANG_VAL=read_ang();
			//printf(" after rotating to the right : %f\n", ANG_VAL);
			//printf("time to move");
			bool_move = move(speed_linear, time, 0,'F');
			rotate_car(90,'R', speed_circular);
			ANG_VAL=read_ang();
			//printf(" \nafter rotating to the left : %f\n", ANG_VAL);
			return 0;
				
		}
		//rotate_car2(90,'R', speed_circular);
		else {
			//go back and consider it as a boundary --> and then we should change thenext direction ?
			rotate_car(180,'L', speed_circular);
			//printf("time to move");
			bool_move = move(speed_linear, bool_move, 0,'F');
			rotate_car(90,'R', speed_circular);
			return max_size+1;
		}
	}
	
}

obstacle_type distinguish_obstacle(void)
	/* written by J.D. */
	/* unknown obstacles, we should also check at the beginning if there is not an obstacle already identified at this position */
{
	int threshold = 50;
	int max_size = 45; //45;
	float old_distance = US_VAL;


	/* for the moment, I consider we are in the small stadium and the obstacles are only those from the teacher */
	int val_color_object = COLOR_VAL;
	update_sensors_value();

	//if (COLOR_VAL == "RED")
	if(strcmp(color[val_color_object],"RED")==0)
	{
		printf("movable\n");        
		return MOVABLE;
	}
	/*if (!detect_obstacle())
	{
		printf("None\n");
		return NONE;
	}*/
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
	if (object_size > max_size)
	{
		printf("boundaries\n");
		    return BOUNDARIES;
	}
	printf("unmovable");
	return UNMOVABLE;
}

/*int main (void )
{
    uint8_t sn_touch;
    int detected_obstacle;
    char Direct ='F';

    printf( "Waiting the EV3 brick online...\n" );
    if ( ev3_init() < 1 )
		return ( 1 );
    printf( "*** ( EV3 ) Hello! ***\n" );
    ev3_sensor_init();
	ev3_tacho_init();
        if(ev3_search_tacho_plugged_in( L_MOTOR_PORT,L_MOTOR_EXT_PORT,motor+L,0))
        {
                get_tacho_max_speed(motor[L],&max_speed);
                set_tacho_command_inx(motor[L],TACHO_RESET);
        } else {
                printf("L NOT Found");
                return (0);
        }
        if(ev3_search_tacho_plugged_in( R_MOTOR_PORT,R_MOTOR_EXT_PORT,motor+R,0))
        {
                set_tacho_command_inx(motor[R],TACHO_RESET);
        } else {
                printf("R NOT Found");
                return (0);
        }
	printf("start2\n");
    //rotate_car2(180,'R', SPEED_CIRCULAR);
	//sleep(1);
    /*move(speed, 10000, 0,'F');
	sleep(1);
    rotate_car2(90,'L', SPEED_CIRCULAR);
	sleep(10);*/
/*    moveinf(speed,Direct);
    printf("Robot stopped because there is an obstacle\n");
	obstacle_type obst = distinguish_obstacle();
	printf("next\n");        
	//printf("%d",obst);
    TOUCHED = _check_pressed( sn_touch);
    if( TOUCHED )
    {
            printf("*** ( EV3 ) Bye! ***\n");
    }
    Sleep(400);

	return 1;
}
*/
