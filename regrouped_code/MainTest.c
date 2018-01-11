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

extern const char const *color[];
extern uint8_t motor[ 3 ];

float x_position = 60.0;
float y_position = 30.0;
int relative_angle = 90;
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

int main (void )
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

  int time = floor(70/velocity)*1000;
  printf("time = %d\n",time);
	int elapsedTime = move(SPEED_LINEAR,time,0,'F');
  Sleep(50);
  printf("ElapsedTime = %d\n",elapsedTime);
  printf("relative angle = %f\n",relative_angle);
  printf("x_position = %f, y_position = %f\n",x_position,y_position);
  rotate_car(90,'L',SPEED_CIRCULAR);
  printf("\nElapsedTime = %d\n",elapsedTime);
  printf("relative angle = %f\n",relative_angle);
  printf("x_position = %f, y_position = %f\n",x_position,y_position);
  move(SPEED_LINEAR,time,1,'F');
  printf("\nElapsedTime = %d\n",elapsedTime);
  printf("relative angle = %f\n",relative_angle);
  printf("x_position = %f, y_position = %f\n",x_position,y_position);
	TOUCHED = _check_pressed( sn_touch);
        if( TOUCHED )
        {
                printf("*** ( EV3 ) Bye! ***\n");
//		break;
        }





	return 0;
}
