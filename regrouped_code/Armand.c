#include <stdio.h>
#include "coroutine.h"
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
#endif
#define SPEED_LINEAR      75  //for linear motion
#define SPEED_CIRCULAR    50  //for circular motion 


#define S_MOTOR_PORT      67
#define S_MOTOR_EXT_PORT  EXT_PORT__NONE_
#define S_OPENING_SPEED		30
#define S_OPENING_ANGLE		60
#define S_OPENING_TIME		1500

bool opened_servo;
bool ball_catched;

void open_servo():
{
	set_tacho_speed_sp(motor[S], S_OPENING_SPEED);
	set_tacho_position_sp(motor[S], DEGREE_TO_COUNT(S_OPENING_ANGLE));
	set_tacho_command_inx(motor[S], S_OPENING_TIME);
	opened_servo = True;
	ball_catched = False;
}

void catch():
{
	speed_circular = max_speed * SPEED_CIRCULAR / 100;
	run_timed(-speed_linear,-speed_linear,1000);
	Sleep(1000);
	rotate(180, 'R', SPEED_CIRCULAR);
	run_timed(speed_linear,speed_linear, 500);
	if (!opened_servo) {
		open_servo();
		Sleep(1000);
	}
	close_servo();
	opened_servo = False;
	ball_catched = True;
}

void close_servo()
{
	set_tacho_speed_sp(motor[S], S_OPENING_SPEED);
	set_tacho_position_sp(motor[S], -DEGREE_TO_COUNT(S_OPENING_ANGLE));
	set_tacho_command_inx(motor[S], S_OPENING_TIME);
	opened_servo = False;
}
