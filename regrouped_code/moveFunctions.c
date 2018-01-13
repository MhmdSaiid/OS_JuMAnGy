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




#ifndef INITIALIZE
#define INITIALIZE
#include "defines.h"
#endif
extern const char const *color[];
extern float x_position, y_position;
extern float relative_angle;
extern int COLOR_VAL;
extern float INTENSITY_VAL;
extern bool TOUCHING;
extern float US_VAL;
extern float ANG_VAL;

extern pthread_mutex_t myMutex;
extern pthread_cond_t obstacleDetected;
extern timeout;


/* Movement functions
*/
void run_forever( int l_speed, int r_speed )
{
/*written by Mohammed Saeed*/
int left_speed = l_speed* MAX_SPEED/100;
int right_speed = r_speed* MAX_SPEED/100;
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
/*written by Mohammed Saeed*/
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
/*written by Mohammed Saeed*/
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
/*written by Mohammed Saeed*/
FLAGS_T state = TACHO_STATE__NONE_;
get_tacho_state_flags( motor[ L ], &state );
if ( state != TACHO_STATE__NONE_ ) return ( 1 );
get_tacho_state_flags( motor[ R ], &state );
if ( state != TACHO_STATE__NONE_ ) return ( 1 );
return ( 0 );
}

void stop_car( void )
{
/*written by Mohammed Saeed*/
multi_set_tacho_command_inx( motor,TACHO_STOP);
}

void old_rotate_car(int angle, char D, int speed_circular)
{
/*written by Mohammed Saeed*/
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


void rotate_car2(int angle,char D,int speed_circular){
	/*written by Justine Deloménie*/
	/*The idea is to improve the previous function in order to be more precise. The solution is to first rotate with the old function and then finish by turning slowly until we reach the desired angle*/
	if(angle<0)
		angle=-angle;
	ANG_VAL=read_ang();
	float start_angle1=ANG_VAL;
	int small_angle = angle/4;
	old_rotate_car(small_angle, D, speed_circular);
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

void rotate_car(float angle,char D, int speed_circular) //Clockwise
{
	/*written by Gautier Dervaux*/
	calibrate_gyro(); // Set currant angle to 0 --> better rotate
	struct timeval startDate;
	int elapsedTime;
	struct timeval stopDate;
	uint8_t nb_values = 10;
	uint8_t i;
	ANG_VAL = read_ang();
	float init_angle = 0;
	printf("angle = %f\n",angle);
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
	relative_angle -= angle; //Gyro opposite direction as gyro
	gettimeofday(&startDate,NULL);
	if(D=='R') run_forever(speed_circular,-speed_circular);
	if(D=='L') run_forever(-speed_circular, speed_circular);

	//rotate_car(angle,D,speed_circular);
	bool turning=true;
	float small_angle_to_turn = 10.0;
	float thresh_angle = angle*(15/90);
	while(1)
	{
		ANG_VAL = read_ang();
		if(D=='R' && ANG_VAL>final_angle-thresh_angle) {
			stop_car();
			gettimeofday(&stopDate,NULL);
			break;
		}
		if(D=='L' && ANG_VAL<(final_angle+thresh_angle)) {
			stop_car();
			gettimeofday(&stopDate,NULL);
			break;
		}
	}
	elapsedTime =(stopDate.tv_sec*1000 + stopDate.tv_usec/1000) - (startDate.tv_sec*1000 + startDate.tv_usec/1000);
	printf("elpased time =%d\n",elapsedTime);
	Sleep(500);
}

int init_motors()
{
/*written by Mohammed Saeed*/
printf("Motors Intitialisation\n");
ev3_tacho_init();
int max_speed;
if(ev3_search_tacho_plugged_in( L_MOTOR_PORT,L_MOTOR_EXT_PORT, motor + L , 0))
{
	get_tacho_max_speed(motor[L],&max_speed);
	set_tacho_command_inx(motor[L],TACHO_RESET);

} else {
	printf("L NOT Found\n");
	return ( 1 );
}
if(ev3_search_tacho_plugged_in( R_MOTOR_PORT,R_MOTOR_EXT_PORT, motor + R,0))
{
	get_tacho_max_speed(motor[R],&max_speed);
	set_tacho_command_inx(motor[R],TACHO_RESET);
} else {
	printf("R NOT Found\n");
	return ( 1 );
}
if(ev3_search_tacho_plugged_in( S_MOTOR_PORT, S_MOTOR_EXT_PORT, motor + S,0))
{
        get_tacho_max_speed(motor[S],&max_speed);
        set_tacho_command_inx(motor[S],TACHO_RESET);
} else {

        return ( 1 );
}

printf("Motors and servo Online\n");
return ( 0 );
}

int move(int speed, int timeInMs,int inf/*If we want to go until an obstacle is found*/,char D){
	/*written by Gautier Dervaux*/
	pthread_mutex_init(&myMutex , NULL ) ;
	pthread_mutex_lock(&myMutex);
	pthread_t sensorsThread;
	struct timespec absDateToStop;
	struct timeval now;
	struct timeval startDate;
	struct timeval stopDate;
	int elapsedTime;
	timeout = false;
	if(D=='B')
	{
		run_timed(-speed, -speed, timeInMs);
		update_position(-timeInMs);//to have a negative distance
		printf("x=%f,y=%f\n",x_position,y_position);
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
		printf("Stopped looking for obstacle\n");
		stop_car();
		timeout = true;
		//pthread_join(&sensorsThread);
		//timeout = false;
		printf("Succedded join\n");
		gettimeofday(&stopDate,NULL);
		//For optimisation purposes the thread checking for obstacle will stop the car if obstacle is detected
		 // Make sure the car is stopped
	}
	else {
		stopReason = pthread_cond_wait(&obstacleDetected, &myMutex);
		stop_car();
		pthread_join(&sensorsThread);
		printf("Succedded join\n");
		gettimeofday(&stopDate,NULL);
	}
	elapsedTime =(stopDate.tv_sec*1000 + stopDate.tv_usec/1000) - (startDate.tv_sec*1000 + startDate.tv_usec/1000);
	update_position(elapsedTime);
	Sleep(500);
	pthread_mutex_unlock(&myMutex); //Obstacle Detected if(inf){ return 1; }
	if(stopReason == 0) return elapsedTime;
	else return 0; //TIMEDOUT
}


/*
void moveinf(int speed,char D){
	//written by Justine Deloménie
	update_sensors_value();
	if(detect_obstacle())//do not start if there is an obstacle in front of us
	{
		stop_car();
		move(SPEED_LINEAR,TIME_BACK,0,'B');
		update_sensors_value();
		return;
	}//otherwise just run
	if(D=='F') run_forever(speed,speed);
	if(D=='B') run_forever(-speed,-speed);
	while(1){
		//check regularly if there is an obstacle. If yes, stop.
        	update_sensors_value();
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
        /*written by Justine Deloménie
	int tacho_count;
        get_tacho_count_per_m(motor[L],&tacho_count);
	if(inf==1){
                moveinf(speed, D);
                return 1;
        }
        else {
		//move by little portions of time in order to check if there is an obstacle between each
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
	/*written by Armand Peron*/
	servo(S_OPENING_SPEED, -S_OPENING_ANGLE);
	ball_catched = 0;
	opened_servo = 1;
}

void catch_object()
{
	/*written by Armand Peron*/
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
	/*written by Armand Peron*/
	servo(S_OPENING_SPEED, S_OPENING_ANGLE);
	opened_servo = 0;
}

void servo( int speed, int angle )
{
	/*written by Armand Peron*/
	set_tacho_speed_sp( motor[ S ], speed );
	set_tacho_position_sp( motor[ S ],DEGREE_TO_COUNT(angle) );
	set_tacho_command_inx( motor[ S ], TACHO_RUN_TO_REL_POS );
	Sleep(1000);
}


void release()
{
	/*written by Armand Peron*/
	open_servo();
	move(SPEED_LINEAR, 1000, 0, 'F');
	close_servo();
}
