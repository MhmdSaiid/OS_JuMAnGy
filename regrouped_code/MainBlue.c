#include <stdio.h> 
#include <stdlib.h>
#include <string.h> 
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
extern const char const *color[];
extern uint8_t motor[ 3 ];

float x_position = 60.0;
float y_position = 0.0;
int relative_angle = 90;
float velocity = 30.0; //25 cm per second for speed linear = 50
float val=3.141592/180.0;
int COLOR_VAL;
float INTENSITY_VAL;
bool TOUCHED;
float US_VAL;
float ANG_VAL;
int *socket_number;


int main (void )
{
    uint8_t sn_touch;
    int detected_obstacle;
    char Direct ='F';
    int rotation_angle=30;
    printf( "Waiting the EV3 brick online...\n" );
    if ( ev3_init() < 1 )
		return ( 1 );
    printf( "*** ( EV3 ) Hello! ***\n" );
    if ( ev3_sensor_init() <1) return ( 1 );
	printf("start2\n");
    if( init_motors() > 0) return ( 1 );
    int s = init_bluetooth_game();
    socket_number = &s;
    int x = 0;
    int y = 0;
    send_position(*socket_number,x,y);
    close_comm(socket_number);
    TOUCHED = _check_pressed( sn_touch);
    if( TOUCHED )
    {
            printf("*** ( EV3 ) Bye! ***\n");
    }
    Sleep(400);

	return 1;
}
