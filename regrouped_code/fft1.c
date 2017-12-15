#include <stdio.h>
#include <stdlib.h>
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
const char const *color[] = { "?", "BLACK", "BLUE", "GREEN", "YELLOW", "RED", "WHITE", "BROWN" };
#define COLOR_COUNT  (( int )( sizeof( color ) / sizeof( color[ 0 ])))

#define L_MOTOR_PORT  	65 //Port A                                                                                                                                                                
#define L_MOTOR_EXT_PORT  EXT_PORT__NONE_                                                                                                                                                            
#define R_MOTOR_PORT  	66 //Port B                                                                                                                                                                
#define R_MOTOR_EXT_PORT EXT_PORT__NONE_ 
#define SPEED_LINEAR  	25  //for linear motion                                                                                                                                                    
#define SPEED_CIRCULAR	50  //for circular motion  	                                                                                                                                            
int max_speed;  /* Motor maximal speed */                                                                                                                                                            
#define DEGREE_TO_COUNT( d )  (( d ) * 260 / 90 ) 
enum { L, R };                                                                                                                                                                                       
uint8_t motor[ 3 ] = { DESC_LIMIT, DESC_LIMIT, DESC_LIMIT };  /* Sequence numbers of motors */                                                     

typedef enum obstacle_type obstacle_type;
enum obstacle_type {ROBOT, UNMOVABLE, MOVABLE, BOUNDARIES, NONE};

static float x_position, y_position;
static int COLOR_VAL;
static float INTENSITY_VAL;
static bool TOUCHED;
static float US_VAL;
static float ANG_VAL;

static bool _check_pressed( uint8_t sn )
{
	int val;

	if ( sn == SENSOR__NONE_ ) {
		return ( ev3_read_keys(( uint8_t *) &val ) && ( val & EV3_KEY_UP ));
	}
	return ( get_sensor_value( 0, sn, &val ) && ( val != 0 ));
}

int read_light_color()
{
	uint8_t sn_color;
	int val;
	while (!( ev3_search_sensor( LEGO_EV3_COLOR, &sn_color, 0 ))) printf("Searching for color sensor...");
	//printf( "COLOR sensor is found, reading COLOR...\n" );
	set_sensor_mode( sn_color, "COL-COLOR" );	
	if ( !get_sensor_value( 0, sn_color, &val ) || ( val < 0 ) || ( val >= COLOR_COUNT )) {		val = 0;
	}
	//printf( "\r(%s) \n", color[ val ]);
	fflush( stdout );
	return val;
}
float read_light_intensity()
{
	uint8_t sn_intensity;
	float value;
	while (!( ev3_search_sensor( LEGO_EV3_COLOR, &sn_intensity, 0 ))) printf("Searching for light intensity sensor...");
	//printf( "Light intensity sensor is found, reading intensity...\n" );
	if ( !get_sensor_value( 0, sn_intensity, &value )) {
		value = 0;
	}
	//printf( "\r Light senses (%d) \n", value);
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
	if ( !get_sensor_value0(sn_compass, &angle )) {
		angle = 0;
	}
	//printf( "\rAngle = (%f) \n", angle);
	fflush( stdout );
	return angle;
}

void update_sensors_value() 
{
COLOR_VAL = read_light_color();
INTENSITY_VAL = read_light_intensity();
//TOUCHED = 
US_VAL = read_US();
ANG_VAL = read_ang();
}



/* Movement functions
*/
static void run_forever( int l_speed, int r_speed )
{
set_tacho_speed_sp( motor[ L ], l_speed );
set_tacho_speed_sp( motor[ R ], r_speed );      	                                                                                                                                                 multi_set_tacho_command_inx( motor, TACHO_RUN_FOREVER ); 
}        

static void stop_car( void )                                                                                                                                                                            
{
multi_set_tacho_command_inx( motor,TACHO_STOP);                                                                                                                                                
}

static void run_timed( int l_speed, int r_speed, int ms )
{
    set_tacho_speed_sp( motor[ L ], l_speed );
    set_tacho_speed_sp( motor[ R ], r_speed );
    multi_set_tacho_time_sp( motor, ms );
    multi_set_tacho_command_inx( motor, TACHO_RUN_TIMED );
}



static void run_to_rel_pos( int l_speed, int l_pos, int r_speed, int r_pos )
{
    set_tacho_speed_sp( motor[ L ], l_speed );
    set_tacho_speed_sp( motor[ R ], r_speed );
    set_tacho_position_sp( motor[ L ], l_pos );
    set_tacho_position_sp( motor[ R ], r_pos );
    multi_set_tacho_command_inx( motor, TACHO_RUN_TO_REL_POS );
}




void rotate(int angle,char D,int speed_circular){

if(angle<0)
  angle=-angle;

if(D=='R')
    run_to_rel_pos( speed_circular, DEGREE_TO_COUNT( +angle )
            , speed_circular, DEGREE_TO_COUNT( -angle ));

if(D=='L')
    run_to_rel_pos( speed_circular, DEGREE_TO_COUNT( -angle )
            , speed_circular, DEGREE_TO_COUNT( +angle ));

}


int app_init( void )
{
    char s[ 16 ];
    if ( ev3_search_tacho_plugged_in( L_MOTOR_PORT, L_MOTOR_EXT_PORT, motor + L, 0 )) {
        get_tacho_max_speed( motor[ L ], &max_speed );
        /* Reset the motor */
        set_tacho_command_inx( motor[ L ], TACHO_RESET );
    } else {
        printf( "LEFT motor (%s) is NOT found.\n", ev3_port_name( L_MOTOR_PORT, L_MOTOR_EXT_PORT, 0, s ));
        /* Inoperative without left motor */
        return ( 0 );
    }
    if ( ev3_search_tacho_plugged_in( R_MOTOR_PORT, R_MOTOR_EXT_PORT, motor + R, 0 )) {
        /* Reset the motor */
        set_tacho_command_inx( motor[ R ], TACHO_RESET );
    } else {
        printf( "RIGHT motor (%s) is NOT found.\n", ev3_port_name( R_MOTOR_PORT, R_MOTOR_EXT_PORT, 0, s ));
        /* Inoperative without right motor */
        return ( 0 );
    }
    
       return ( 1 );
}




int main (void )
{
	printf( "Waiting the EV3 brick online...\n" );
    if ( ev3_init() < 1 ) return ( 1 );

    printf( "*** ( EV3 ) Hello! ***\n" );

    ev3_sensor_init();
    ev3_tacho_init();
if(app_init()==0)
return 0;


int speed_linear, speed_circular,angle;
speed_linear = max_speed * SPEED_LINEAR / 100;
speed_circular = max_speed * SPEED_CIRCULAR / 100;
angle=30;
int x=0,y=0;

while(1){
run_forever(speed_linear, speed_linear );
update_sensors_value();
printf("US=%f\n",US_VAL);
if(US_VAL<=65){

stop_car();
update_sensors_value();	
if(strcmp(color[COLOR_VAL],"RED")==0)
printf("This is a RED thing.\n");
Sleep(500);
rotate(30,'R',speed_circular);
Sleep(1000);
run_forever(speed_linear, speed_linear );
Sleep(1000);
}
}
/*


//Move Forward
run_timed(-speed_linear,-speed_linear,1000);
printf("x=%d,y=%d\n",x+1000/1000*speed_linear,y+1000/1000*speed_linear);

Sleep(2000);

//Move Backwards
run_timed(speed_linear,speed_linear,1000);
Sleep(2000);

//Turn Right
run_timed(speed_circular,-speed_circular,1000);
Sleep(2000);

//Turn Left
run_timed(-speed_circular,speed_circular,1000);
Sleep(2000);

//Turn 30 degrees anticlockwise
 run_to_rel_pos( speed_circular, DEGREE_TO_COUNT( -angle )
            , speed_circular, DEGREE_TO_COUNT( angle ));
Sleep(2000);

//Turn 180 degrees anticlockwise
angle=180;
 run_to_rel_pos( speed_circular, DEGREE_TO_COUNT( -angle )
            , speed_circular, DEGREE_TO_COUNT( angle ));
Sleep(2000);
    
rotate(30,'R',speed_circular);

Sleep(2000);

rotate(30,'L',speed_circular);
Sleep(2000);
*/

             


    ev3_uninit();
    printf( "*** ( EV3 ) Bye! ***\n" );
    return ( 0 );
}







