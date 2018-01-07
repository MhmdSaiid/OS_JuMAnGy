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

bool _check_pressed( uint8_t sn )
{
	int val;

	if ( sn == SENSOR__NONE_ ) {
		return ( ev3_read_keys(( uint8_t *) &val ) && ( val & EV3_KEY_UP ));
	}
	return ( get_sensor_value( 0, sn, &val ) && ( val != 0 ));
}
//yolo
int read_light_color()
{
	uint8_t sn_color;
	int val;
	while (!( ev3_search_sensor( LEGO_EV3_COLOR, &sn_color, 0 ))) printf("Searching for color sensor...");
	//printf( "COLOR sensor is found, reading COLOR...\n" );
	set_sensor_mode( sn_color, "COL-COLOR" );
	if ( !get_sensor_value( 0, sn_color, &val ) || ( val < 0 ) || ( val >= COLOR_COUNT )) {
		val = 0;
	}
	//printf( "\r(%s) \n", color[ val ]);
	fflush( stdout );
	return val;
}

float read_light_intensity()
{
	uint8_t sn_intensity;
	float value;
	while (!( ev3_search_sensor( LEGO_EV3_COLOR, &sn_intensity, 0 ))) printf("Searching for light intensity sensor...\n");
	//printf( "Light intensity reflection sensor is found, reading value..\n");
	set_sensor_mode( sn_intensity, "COL-REFLECT" );
	if ( !get_sensor_value0(sn_intensity, &value )) {
		value = 0;
	}
	//printf("\r Light senses (%f) \n",value);
	fflush( stdout );
	return value;
}


float read_US()
{
	uint8_t sn_sonar;
	float value;
	while (!(ev3_search_sensor(LEGO_EV3_US, &sn_sonar,0))) printf("Searching for US sensor...");
	//printf("SONAR found, reading sonar...\n");
	if ( !get_sensor_value0(sn_sonar, &value )) {
		value = 0;
	}
	//printf( "\r US senses (%f) \n", value);
	fflush( stdout );
	return value;
}
float read_ang()
{
	uint8_t sn_compass;
	float angle;
	while (!(ev3_search_sensor(LEGO_EV3_GYRO, &sn_compass,0))) printf("Searching for Compass sensor...");
	//printf("COMPASS found, reading compass...\n");
	if ( !get_sensor_value0( sn_compass, &angle )) {
		angle = 0;
	}
	//printf( "\rAngle = (%f) \n", angle);
	fflush( stdout );
	return angle;
}

void calibrate_gyro() // set as 0 currant angle --> better rotations
{
	uint8_t sn_compass;
	while (!(ev3_search_sensor(LEGO_EV3_GYRO, &sn_compass,0))) printf("Searching for Compass sensor...");
	set_sensor_mode( sn_compass, "GYRO-CAL" );
	ANG_VAL = read_ang();
	Sleep(100);
	set_sensor_mode( sn_compass, "GYRO-ANG" );
}

void update_sensors_value()
{
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
	//printf("Hello from the thread\n");
	uint8_t sn_touch;
	long int i=0;
	update_sensors_value();
	while (!detect_obstacle())
	{
		update_sensors_value();
		i++;
	}
	//printf("Nb Check = %d\n",i);
	printf("Obstacle Detected!!\n ");
	stop_car();
	pthread_cond_signal(&obstacleDetected);
	printf("Hello from thread\n");
	exit;
}
