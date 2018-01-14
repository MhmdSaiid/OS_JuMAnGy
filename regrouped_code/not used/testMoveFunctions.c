#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ev3.h"
#include "ev3_port.h"
#include "ev3_sensor.h"
#include "moveFunctions.h"

// WIN32 /////////////////////////////////////////
#ifdef __WIN32__

#include <windows.h>

// UNIX //////////////////////////////////////////
#else

#include <unistd.h>
#define Sleep( msec ) usleep(( msec ) * 1000 )

//////////////////////////////////////////////////
#endif


//const char const *color[] = { "?", "BLACK", "BLUE", "GREEN", "YELLOW", "RED", "WHITE", "BROWN" };
//#define COLOR_COUNT  (( int )( sizeof( color ) / sizeof( color[ 0 ])))

#define SPEED_LINEAR 10 //for linear motion
#define SPEED_CIRCULAR 25 //for linear motion



int main( void )
{
	//uint8_t sn_touch;
	printf( "Waiting the EV3 brick online...\n" );
	if ( ev3_init() < 1 ) return ( 1 );
	printf( "*** ( EV3 ) Hello! ***\n" );
	printf("Initialising motors \n");
	ev3_tacho_init();
	if ( motorsInit() > 0) return ( 1 );
		for ( ; ; ) {
			//update_sensors_value();
			printf("Moving... \n");
			run_forever(SPEED_LINEAR,SPEED_LINEAR);
			printf("Stoping... \n");
			Sleep(400);
			stop_car();
		}
	return ( 0 );
}


