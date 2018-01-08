#include <stdio.h>
#include <stdlib.h>
#include "ev3.h"
#include "ev3_port.h"
#include "ev3_tacho.h"
#include <unistd.h>
#include <time.h>
#ifndef MOVE_FUNC
#define MOVE_FUNC
#include "moveFunctions.h"
#endif
#ifndef READ_FUNC
#define READ_FUNC
#include "readFunctions.h"
#endif

uint8_t motor[ 3 ] = { DESC_LIMIT, DESC_LIMIT, DESC_LIMIT };  /* Sequence numbers of motors */
enum { L, R, S};
// WIN32 /////////////////////////////////////////
/*
#ifdef __WIN32__

#include <windows.h>

// UNIX //////////////////////////////////////////
#else

#include <unistd.h>
#define Sleep( msec ) usleep(( msec ) * 1000 )

//////////////////////////////////////////////////
#endif
*/



#ifndef INITIALIZE
#define INITIALIZE
#include "defines.h"
#endif
extern const char const *color[];
extern float x_position, y_position;
extern int relative_angle;
extern int COLOR_VAL;
extern float INTENSITY_VAL;
extern bool TOUCHING;
extern float US_VAL;
extern float ANG_VAL;

extern pthread_mutex_t myMutex;
extern pthread_cond_t obstacleDetected;


/* Movement functions
*/
void run_forever( int l_speed, int r_speed )
{
printf("run forever\n");
int left_speed = l_speed* MAX_SPEED/100;
int right_speed = r_speed* MAX_SPEED/100;
//printf("left speed =%d \n",left_speed);
set_tacho_speed_sp( motor[ L ], left_speed );
set_tacho_speed_sp( motor[ R ], right_speed);

//Either we set the S motor speed at 0 before setting command for all
/*set_tacho_speed_sp( motor[ S ], 0);
multi_set_tacho_command_inx( motor, TACHO_RUN_FOREVER );
*/
//Either we just set the command for other motors
set_tacho_command_inx( motor[L], TACHO_RUN_FOREVER);
set_tacho_command_inx( motor[R], TACHO_RUN_FOREVER);
}

void run_to_rel_pos(int l_speed,int l_pos,int r_speed,int r_pos)
{
int left_speed = l_speed *MAX_SPEED/100;
int right_speed = r_speed *MAX_SPEED/100;
set_tacho_speed_sp( motor[ L ], left_speed );
set_tacho_speed_sp( motor[ R ], right_speed );
set_tacho_position_sp( motor[ L ], l_pos );
set_tacho_position_sp( motor[ R ], r_pos );

//Same as in run_forever function
/*set_tacho_position_sp( motor[ S ], 0);
multi_set_tacho_command_inx( motor, TACHO_RUN_TO_REL_POS);
*/
set_tacho_command_inx( motor[ L ], TACHO_RUN_TO_REL_POS );
set_tacho_command_inx( motor[ R ], TACHO_RUN_TO_REL_POS );
}


void run_timed( int l_speed, int r_speed, int ms )
{
int left_speed = l_speed * MAX_SPEED/100;
int right_speed = r_speed * MAX_SPEED/100;
set_tacho_speed_sp( motor[ L ], left_speed );
set_tacho_speed_sp( motor[ R ], right_speed );
multi_set_tacho_time_sp( motor, ms );

//Same as inf run_forever function
/*set_tacho_speed_sp( motor[S], 0);
multi_set_tacho_command_inx(motor, TACHO_RUN_TIMED);
*/
set_tacho_command_inx( motor[ L ], TACHO_RUN_TIMED );
set_tacho_command_inx( motor[ R ], TACHO_RUN_TIMED );
Sleep(ms);
}


int is_running( void )
{
FLAGS_T state = TACHO_STATE__NONE_;
get_tacho_state_flags( motor[ L ], &state );
if ( state != TACHO_STATE__NONE_ ) return ( 1 );
get_tacho_state_flags( motor[ R ], &state );
if ( state != TACHO_STATE__NONE_ ) return ( 1 );
return ( 0 );
}

void stop_car( void )
{
multi_set_tacho_command_inx( motor,TACHO_STOP);
}

void old_rotate_car(int angle, char D, int speed_circular)
{
if (angle<0)
	angle=-angle;
if(D=='R')
    {
    run_to_rel_pos( speed_circular, DEGREE_TO_COUNT( +angle ) , speed_circular, DEGREE_TO_COUNT( -angle ));
    relative_angle -= angle;
    }
if(D=='L')
	{
	run_to_rel_pos( speed_circular, DEGREE_TO_COUNT( -angle ) , speed_circular, DEGREE_TO_COUNT( +angle ));
	relative_angle += angle;
	}
while(is_running());

}

void rotate_car(int angle,char D, int speed_circular) //Clockwise
{
	calibrate_gyro(); // Set currant angle to 0 --> better rotate
	uint8_t nb_values = 10;
	uint8_t i;
	ANG_VAL = read_ang();
	float init_angle = 0;
	printf("Init ANG VAL = %f\n",init_angle);
	printf("Angle wanted = %d\n",angle);
	printf("Direction = %c\n",D);
	angle = abs(angle);
	float final_angle;
	if(D=='L')//Sensor value increase clockwise
	{
		final_angle = init_angle - angle;
		angle = -angle;
	}
	else
	{
		final_angle = init_angle + angle;
	}
	relative_angle += angle;

	if(D=='R') run_forever(speed_circular,-speed_circular);
	if(D=='L') run_forever(-speed_circular, speed_circular);

	//rotate_car(angle,D,speed_circular);
	bool turning=true;
	float small_angle_to_turn = 10.0;
	while(1)
	{
		ANG_VAL = read_ang();
		if(D=='R' && ANG_VAL>final_angle-5) {
			printf("Stoppin Right");
			stop_car();
			break;
		}
		if(D=='L' && ANG_VAL<final_angle+5) {
			printf("Stoppin Left");
			stop_car();
			break;
		}
	}
	printf("Final angle = %f, Final compass angle =%f",final_angle,ANG_VAL);
}


void rotate_car2(int angle,char D,int speed_circular){
	/*written by J.D.*/
	if(angle<0)
		angle=-angle;
	ANG_VAL=read_ang();
	float start_angle1=ANG_VAL;
	int small_angle = angle/4;
	rotate_car(small_angle, D, speed_circular);
	ANG_VAL=read_ang();
	angle=angle-abs(ANG_VAL-start_angle1);
	int angle_unit=1; //Turns 3°deg per deg

	ANG_VAL=read_ang();
	float start_angle=ANG_VAL;
	float new_angle=ANG_VAL;
	int compass_range = 2;
	//while the rotation we made is not equal to the asked angle+-3 degrees
	while(abs(new_angle-start_angle) > angle+compass_range || abs(new_angle-start_angle) < angle +compass_range ) {
		if(D=='R'){

	    		run_to_rel_pos( speed_circular, DEGREE_TO_COUNT( +angle_unit ), speed_circular, DEGREE_TO_COUNT( -angle_unit ));
		}

		if(D=='L'){
	    		run_to_rel_pos( speed_circular, DEGREE_TO_COUNT( -angle_unit ), speed_circular, DEGREE_TO_COUNT( +angle_unit ));
		}
		ANG_VAL=read_ang();
		new_angle=ANG_VAL;
	}

}



int init_motors()
{
printf("Motors Intitialisation\n");
ev3_tacho_init();
int max_speed;
if(ev3_search_tacho_plugged_in( L_MOTOR_PORT,L_MOTOR_EXT_PORT, motor + L , 0))
{
	get_tacho_max_speed(motor[L],&max_speed);
//        printf("max speed is %d \n", max_speed);
	set_tacho_command_inx(motor[L],TACHO_RESET);
//	printf("Left Motor Done ! \n");
} else {
	printf("L NOT Found\n");
	return ( 1 );
}
if(ev3_search_tacho_plugged_in( R_MOTOR_PORT,R_MOTOR_EXT_PORT, motor + R,0))
{
	get_tacho_max_speed(motor[R],&max_speed);
//        printf("max speed is %d \n", max_speed);
	set_tacho_command_inx(motor[R],TACHO_RESET);
//	printf("Right Motor Done! \n");
} else {
	printf("R NOT Found\n");
	return ( 1 );
}
if(ev3_search_tacho_plugged_in( S_MOTOR_PORT, S_MOTOR_EXT_PORT, motor + S,0))
{
        get_tacho_max_speed(motor[S],&max_speed);
//        printf("max speed is %d \n", max_speed);
        set_tacho_command_inx(motor[S],TACHO_RESET);
//        printf("Servo Motor Done! \n");
} else {
//        printf("S NOT Found\n");
        return ( 1 );
}

printf("Motors and servo Online\n");
return ( 0 );
}

int move(int speed, int timeInMs,int inf/*If we want to go until an obstacle is found*/,char D){
	pthread_mutex_init(&myMutex , NULL ) ;
	pthread_mutex_lock(&myMutex);
	pthread_t sensorsThread;
	struct timespec absDateToStop;
	struct timeval now;
	struct timeval startDate;
	struct timeval stopDate;
	int elapsedTime;
	if(D=='B')
	{
		run_timed(-speed, -speed, timeInMs);
		update_position(-timeInMs);//to have a negative distance
		return 0;
	}
	//Calculating when the car must stop
	gettimeofday(&now,NULL);
	absDateToStop.tv_sec = time(NULL) + timeInMs / 1000;
	absDateToStop.tv_nsec = now.tv_usec * 1000 + 1000 * 1000 * (timeInMs % 1000);
	absDateToStop.tv_sec += absDateToStop.tv_nsec / (1000 * 1000 * 1000);
	absDateToStop.tv_nsec %= (1000 * 1000 *1000);
	int stopReason;
	if(pthread_create(&sensorsThread, NULL, &readingSensors, NULL))
	{
		printf("Failed creating thread to detect obstacle\n");
		return(2);
	}
	gettimeofday(&startDate,NULL);
	run_forever(speed,speed);
	if(inf==0) {
		stopReason = pthread_cond_timedwait(&obstacleDetected, &myMutex, &absDateToStop);
		gettimeofday(&stopDate,NULL);
		//For optimisation purposes the thread checking for obstacle will stop the car if obstacle is detected
		stop_car(); // Make sure the car is stopped
	}
	else {
		stopReason = pthread_cond_wait(&obstacleDetected, &myMutex);
		stop_car();
		gettimeofday(&stopDate,NULL);
	}
	elapsedTime =(stopDate.tv_sec*1000 + stopDate.tv_usec/1000) - (startDate.tv_sec*1000 + startDate.tv_usec/1000);
	update_position(elapsedTime);
	pthread_join(sensorsThread);
	pthread_mutex_unlock(&myMutex); //Obstacle Detected if(inf){ return 1; }
	if(stopReason == 0) return elapsedTime;
	else return 0; //TIMEDOUT
}

//test
/*
void moveinf(int speed,char D){
	//printf("moveinf before update\n");
	update_sensors_value();
	//printf("moveinf after update\n");
	if(detect_obstacle())
	{
		stop_car();
		move(SPEED_LINEAR,TIME_BACK,0,'B');
		update_sensors_value();
		return;
	}
	if(D=='F') run_forever(speed,speed);
	if(D=='B') run_forever(-speed,-speed);
	while(1){
        	update_sensors_value();
		//printf("still in while");
		if(detect_obstacle())
        	{
                	stop_car();
			int time = 1000;
			move(SPEED_LINEAR,TIME_BACK,0,'B');
			update_sensors_value();
                	break;
        	}
		int time_loop = 50;
        	Sleep(time_loop);
		update_position(time_loop);
	}
	return;
}

int move(int speed, int time, int inf, char D){
        /*written by J.D.
	int tacho_count;
        get_tacho_count_per_m(motor[L],&tacho_count);
        //printf("count per m = %d\n",tacho_count);
	if(inf==1){
                moveinf(speed, D);
                return 1;
        }
        else {
                int time_elapsed=0;
                int partialTime= 100 ; //).2s
                while(time_elapsed < time){
                        update_sensors_value();
                        if(D=='F')
			{
				if(detect_obstacle()){
					stop_car();
					move(SPEED_LINEAR,TIME_BACK,0,'B');
					update_sensors_value();
					return time_elapsed;
                        	}
                        	run_timed(speed, speed, partialTime);
				update_position(partialTime);//to have a negative distance

			}
                        if(D=='B')
			{
				run_timed(-speed, -speed, partialTime);
				update_position(-partialTime);//to have a negative distance
			}
	                time_elapsed=time_elapsed + partialTime;
                }
                return 0;


			}
}

*/
int opened_servo = 0;
int ball_catched;
void open_servo()
{
	servo(S_OPENING_SPEED, -S_OPENING_ANGLE);
	ball_catched = 0;
	opened_servo = 1;
}

void catch_object()
{
	move(SPEED_LINEAR,400,0,'B');
	if (opened_servo==0)
	{
		open_servo();
		Sleep(500);
	}
        rotate_car(180, 'R', SPEED_CIRCULAR);
	move(SPEED_LINEAR/2, BACK_TIME_TO_CATCH_BALL*2, 0, 'B');
	close_servo();
	opened_servo = 0;
	ball_catched = 1;
}

void close_servo()
{
	servo(S_OPENING_SPEED, S_OPENING_ANGLE);
	opened_servo = 0;
}

void servo( int speed, int angle )
{
	set_tacho_speed_sp( motor[ S ], speed );
	set_tacho_position_sp( motor[ S ],DEGREE_TO_COUNT(angle) );
	set_tacho_command_inx( motor[ S ], TACHO_RUN_TO_REL_POS );
	Sleep(1000);
}


void release()
{
	open_servo();
	move(SPEED_LINEAR, 1000, 0, 'F');
	close_servo();
}
