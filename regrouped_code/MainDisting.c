#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

extern const char const *color[];
extern uint8_t motor[ 3 ];

float x_position = 60.0;
float y_position = 30.0;
int relative_angle = 90;
float velocity = 30.0; //25 cm per second for speed linear = 50
float val=3.141592/180.0;
int COLOR_VAL;
float INTENSITY_VAL;
bool TOUCHED;
float US_VAL;
float ANG_VAL;
bool TOUCHING;
int socket_number; //Defined by the thread that handles the socket


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
	//socket_number = init_bluetooth_game();
	//socket_number = 0;
	//printf("Socket created\n");
	//create thread handling sockets
	//pthread_t sendingthread;
	/*if(pthread_create(&sendingthread, NULL, sending_process, NULL)) {
                printf("WASTED\n");
                return(2);
        }
	printf("thread launched\n");*/
	//main code
        //release();
	//while(1){
		moveinf(SPEED_LINEAR,Direct);
                printf("Robot stopped because there is an obstacle\n");
                obstacle_type obst = distinguish_obstacle();
		//if(obst==2) catch_object();
		//rotate_car(100, 'R', SPEED_CIRCULAR);
		TOUCHED = _check_pressed( sn_touch);
                if( TOUCHED )
                {
                        printf("*** ( EV3 ) Bye! ***\n");
			//break;
                }
	//}
	//close thread
        /*printf("Terminating thread... \n ");
        pthread_cancel(sendingthread);
        printf("thread TERMINATED\n");
	
	//close connection
	printf("Close communication \n");
	close_comm(socket_number);
*/


	return 0;
}
