#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ev3.h"
#include "ev3_port.h"
#include "ev3_sensor.h"
#include "readFunctions.h"

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



static float x_position,y_position;
static char COLOR_VAL;
static float INTENSITY_VAL;
static bool TOUCHED;
static float US_VAL;
static float ANG_VAL;
// extern MAP*/
void update_sensors_value()
{
	COLOR_VAL = read_light_color();
	INTENSITY_VAL = read_light_intensity();
	US_VAL = read_US();
	ANG_VAL = read_ang();
}

int main( void )
{
	uint8_t sn_touch;
	printf( "Waiting the EV3 brick online...\n" );
	if ( ev3_init() < 1 ) return ( 1 );
	printf( "*** ( EV3 ) Hello! ***\n" );
	ev3_sensor_init();
		for ( ; ; ) {
			update_sensors_value();
			Sleep(200);
			if ( _check_pressed(sn_touch)) return ( 0 );

		}
	return ( 0 );
}


