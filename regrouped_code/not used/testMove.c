#include <stdio.h>
#include <stdlib.h>
#include "ev3.h"
#include "ev3_port.h"
#include "ev3_sensor.h"
#include "ev3_tacho.h"
#include "readFunctions.h"
#include "moveFunctions.h"
#include "detect_disting_functions.h"
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

#define SPEED_LINEAR  	25  //for linear motion
#define SPEED_CIRCULAR	50  //for circular motion
#define DEGREE_TO_COUNT( d )  (( d ) * 260 / 90 ) 

extern float x_position, y_position;
extern char COLOR_VAL;
extern float INTENSITY_VAL;
extern bool TOUCHED;
extern float US_VAL;
extern float ANG_VAL;

void update_sensors_value() 
{
COLOR_VAL = read_light_color();
INTENSITY_VAL = read_light_intensity();
//TOUCHED = 
US_VAL = read_US();
ANG_VAL = read_ang();
}
void moveTest(int speed,char D)
{
	if(D=='F') run_forever(speed,speed);
	if(D=='B') run_forever(-speed,-speed);
	while(1)
	{
		update_sensors_value();
		if(detect_obstacle())
		{
			stop_car();
			break;
		}
	}
}
int main( void )
{
	uint8_t sn_touch;
	int detected_obstacle;
	char Direct ='F';
        //max_speed=1050;
	//int speed = max_speed*SPEED_LINEAR/100;
	printf( "Waiting the EV3 brick online...\n" );
	if ( ev3_init() < 1 ) return ( 1 );
	printf( "*** ( EV3 ) Hello! ***\n" );
	
	if ( ev3_sensor_init() <1) return ( 1 );

	if( init_motors() > 0) return ( 1 );
	printf("Moving...\n");
	moveTest(SPEED_LINEAR,Direct);
	int angle = 180;
	char rotationD = 'R';
	rotate_car(angle,rotationD,SPEED_CIRCULAR);
}

