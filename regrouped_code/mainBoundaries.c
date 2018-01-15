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
#ifndef BOUND
#define BOUND
#include "Boundaries.h"
#endif

#ifndef LINKED
#define LINKED
#include "linkedlist.h"
#endif


//#include "Boundaries.c"
extern const char const *color[];
extern uint8_t motor[ 3 ];

float x_position = 60.0;
float y_position = 30.0;
float relative_angle = 0;
float velocity = SPEED_LINEAR/2; //25 cm.s per second for speed linear = 50
float val=3.141592/180.0;
int COLOR_VAL;
float INTENSITY_VAL;
bool TOUCHED;
float US_VAL;
float ANG_VAL;
bool TOUCHING;
int socket_number; //Defined by the thread that handles the socket
pthread_mutex_t myMutex;
pthread_cond_t obstacleDetected;
bool timeout = false;
uint8_t * map;
boundary_t * boundaries=NULL;
int finished=0;
extern float XMAX;
extern float YMAX;

int main(void)
{
	uint8_t sn_touch;
	int detected_obstacle;
	char Direct ='F';
	int rotation_angle=30;
	int abs_speed_servo = MAX_SPEED * SPEED_SERVO / 100;
	printf( "Waiting the EV3 brick online...\n" );
	if ( ev3_init() < 1 )
		return ( 1 );
	printf( "*** ( EV3 ) Hello! ***\n" );
	if( ev3_sensor_init() < 1) return ( 1 );
	printf("Sensors online\n");
	if( init_motors() > 0) return ( 1 );


	//moveinf(SPEED_LINEAR,Direct);
        //printf("Robot stopped because there is an obstacle\n");
        //int obst = distinguish_obstacle();
	//if(obst==3) catch_object();
	//rotate_car(100, 'R', SPEED_CIRCULAR);
	//limitObst();
	//move(SPEED_LINEAR,0,1,Direct);
	//scouting();
	//rotate_car(90,'R',SPEED_CIRCULAR);
//	move(SPEED_LINEAR,0,1,'F');
//	find_right_angle_obst();

	//map  = small_stadium_map(120,100);
	bound();
	printf("DEBUG bound finished getSize incomming\n");
	getSize(boundaries);
	if(XMAX==0 || YMAX==0)
	{
		x_position = 10.0;
		map = small_stadium_map(80*5,80*5);
	}
	else{
	printf("GET SIZE DONE\n");
	map = initializeMap(boundaries);
	printf("DEBUG map Initialized\n");
	}
	print_map(map);
	

	//scouting();
	TOUCHED = _check_pressed( sn_touch);
        if( TOUCHED )
        {
                printf("*** ( EV3 ) Bye! ***\n");
//		break;
        }





	return 0;
}
