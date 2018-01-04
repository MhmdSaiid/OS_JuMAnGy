#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "math.h"
#include <time.h>
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

#ifndef THREADSENSORS
#define THREADSENSORS
#include "sensorsThread.h"
#endif


extern const char const *color[];
extern uint8_t motor[ 3 ];

float x_position = 60.0;
float y_position = 30.0;
int relative_angle = 90;
float velocity = 20.0; //25 cm per second for speed linear = 50
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

int main (void )
{
	uint8_t sn_touch;
	printf( "Waiting the EV3 brick online...\n" );
	if ( ev3_init() < 1 )
		return ( 1 );
	printf( "*** ( EV3 ) Hello! ***\n" );
	if( ev3_sensor_init() < 1) return ( 1 );
	printf("Sensors online\n");
	if( init_motors() > 0) return ( 1 );

	//main code
	int test;
	int rotation_angle=90;
	ANG_VAL=read_ang();
	printf("InitAng val = %f\n",ANG_VAL);
	moveThread(SPEED_LINEAR, 5000, 0, 'F');
	find_right_angle_obst();
	find_right_angle_obst();
	/*
	rotate_carPP(90,'R',SPEED_CIRCULAR);
	*/
	return 0;
}
