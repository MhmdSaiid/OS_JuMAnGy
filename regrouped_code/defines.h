#define Sleep( msec ) usleep(( msec ) * 1000 )

#define COLOR_COUNT  (( int )( sizeof( color ) / sizeof( color[ 0 ])))

#define L_MOTOR_PORT    65 //Port A
#define L_MOTOR_EXT_PORT  EXT_PORT__NONE_
#define R_MOTOR_PORT    66 //Port B
#define R_MOTOR_EXT_PORT EXT_PORT__NONE_
#define S_MOTOR_PORT    67 //Port C
#define S_MOTOR_EXT_PORT EXT_PORT__NONE_

#define SPEED_LINEAR    35 //50  //for linear motion
#define SPEED_CIRCULAR  10  //for circular motion
#define SPEED_SERVO	30
#define DEGREE_TO_COUNT( d )  (( d ) * 180 / 90 )
#define MAX_SPEED 	1050  /* Motor maximal speed */


#define S_OPENING_SPEED	300
#define S_OPENING_ANGLE	40
#define S_OPENING_TIME	1500

#define BACK_TIME_TO_CATCH_BALL	1000
#define TIME_BACK 120 //20

#define UNEXPLORED 0
#define EMPTY 1
#define UNMOVABLE 2
#define MOVABLE 3
#define BOUNDARIES 4
#define ROBOT 5
#define NONE 6
