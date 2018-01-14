#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "ev3.h"
#include "ev3_port.h"
#include "ev3_sensor.h"
#ifndef READ_FUNC
#define READ_FUNC
#include "readFunctions.h"
#endif

const char const *color[] = { "?", "BLACK", "BLUE", "GREEN", "YELLOW", "RED", "WHITE", "BROWN" };
// WIN32 /////////////////////////////////////////
#ifdef __WIN32__

#include <windows.h>

// UNIX //////////////////////////////////////////
#else

#include <unistd.h>
//////////////////////////////////////////////////
#endif


#ifndef INITIALIZE
#define INITIALIZE
#include "defines.h"

#endif

extern int max_speed;
extern float x_position, y_position;
extern int COLOR_VAL;
extern float INTENSITY_VAL;
extern bool TOUCHED;
extern float US_VAL;
extern float ANG_VAL;
extern bool TOUCHING;
extern uint8_t motor[ 3 ];
extern pthread_mutex_t myMutex;
extern pthread_cond_t obstacleDetected;
extern bool timeout;

bool _check_pressed( uint8_t sn )
{
/*written by Gautier Dervaux*/
	int val;

	if ( sn == SENSOR__NONE_ ) {
		return ( ev3_read_keys(( uint8_t *) &val ) && ( val & EV3_KEY_UP ));
	}
	return ( get_sensor_value( 0, sn, &val ) && ( val != 0 ));
}

int read_light_color()
{
	/*written by Gautier Dervaux*/
	uint8_t sn_color;
	int val;
	while (!( ev3_search_sensor( LEGO_EV3_COLOR, &sn_color, 0 ))) printf("Searching for color sensor...");
	set_sensor_mode( sn_color, "COL-COLOR" );
	if ( !get_sensor_value( 0, sn_color, &val ) || ( val < 0 ) || ( val >= COLOR_COUNT )) {
		val = 0;
	}
	fflush( stdout );
	return val;
}

float read_light_intensity()
{
	/*written by Gautier Dervaux*/
	uint8_t sn_intensity;
	float value;
	while (!( ev3_search_sensor( LEGO_EV3_COLOR, &sn_intensity, 0 ))) printf("Searching for light intensity sensor...\n");
	set_sensor_mode( sn_intensity, "COL-REFLECT" );
	if ( !get_sensor_value0(sn_intensity, &value )) {
		value = 0;
	}
	fflush( stdout );
	return value;
}


float read_US()
{
	/*written by Gautier Dervaux*/
	uint8_t sn_sonar;
	float value;
	while (!(ev3_search_sensor(LEGO_EV3_US, &sn_sonar,0))) printf("Searching for US sensor...");
	if ( !get_sensor_value0(sn_sonar, &value )) {
		value = 0;
	}
	fflush( stdout );
	return value;
}
float read_ang()
{
	/*written by Gautier Dervaux*/
	uint8_t sn_compass;
	float angle;
	while (!(ev3_search_sensor(LEGO_EV3_GYRO, &sn_compass,0))) printf("Searching for Compass sensor...");
	if ( !get_sensor_value0( sn_compass, &angle )) {
		angle = 0;
	}
	fflush( stdout );
	return angle;
}

void calibrate_gyro() // set as 0 currant angle --> better rotations
{
	/*written by Gautier Dervaux*/
	uint8_t sn_compass;
	while (!(ev3_search_sensor(LEGO_EV3_GYRO, &sn_compass,0))) printf("Searching for Compass sensor...");
	set_sensor_mode( sn_compass, "GYRO-CAL" );
	ANG_VAL = read_ang();
	Sleep(100);
	set_sensor_mode( sn_compass, "GYRO-ANG" );
}

void update_sensors_value()
{
	/*written by Gautier Dervaux*/
	//printf("Updating\n");
	uint8_t sn_touch;
	COLOR_VAL = read_light_color();
	INTENSITY_VAL = read_light_intensity();
	US_VAL = read_US();
	ANG_VAL = read_ang();
	TOUCHING = _check_pressed( sn_touch);

}




void *readingSensors(void)
{
	/*written by Gautier Dervaux*/
	printf("Hello from the thread\n");
	uint8_t sn_touch;
	long int i=0;
	update_sensors_value();
	Sleep(100);
	while (!detect_obstacle())
	{
		update_sensors_value();
		if(timeout)
		{
			printf("Timedout so stopping\n");
			if(is_running()) stop_car();
			return;
		}
		i++;
	}
	printf("Obstacle Detected!!\n ");
	pthread_cond_signal(&obstacleDetected);
	return;
}
