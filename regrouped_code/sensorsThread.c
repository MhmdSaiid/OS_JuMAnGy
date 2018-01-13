//#include <stdio.h>
//#include <stdlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>


#include "ev3.h"
#include "ev3_port.h"
#include "ev3_sensor.h"
#ifndef INITIALIZE
#define INITIALIZE
#include "defines.h"
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

#ifndef SENSORSTHREAD
#define SENSORSTHREAD
#include "sensorsThread.h"
#endif


//#define Sleep(msec) usleep ((msec)*1000)
extern float x_position;
extern float y_position;

extern int COLOR_VAL;
extern float INTENSITY_VAL;
extern bool TOUCHED;
extern float US_VAL;
extern float ANG_VAL;
extern bool TOUCHING;

extern int socket_number;
extern pthread_cond_t obstacleDetected;

extern pthread_mutex_t myMutex;
extern pthread_cond_t obstacleDetected;
/*
void *readingSensors(void) {
        printf("Hello from the thread\n");
	uint8_t sn_touch;
	long int i=0;
	update_sensors_value();
        while (!detect_obstacle()) {
		update_sensors_value();
		i++;

        }
	printf("Nb Check = %d\n",i);
	printf("Obstacle Detected!!\n ");
	stop_car();
	pthread_cond_signal(&obstacleDetected);
}
*/
int moveThread(int speed, int timeInMs,int inf/*If we want to go until an obstacle is found*/,char D){
	/*written by Gautier Dervaux*/
	pthread_mutex_init(&myMutex , NULL ) ;
	pthread_mutex_lock(&myMutex);
	pthread_t sensorsThread;
	struct timespec absDateToStop;
	struct timeval now;
	struct timeval startDate;
	struct timeval stopDate;
	int elapsedTime;
	if(D=='B')
	{
		run_timed(-speed, -speed, timeInMs);
		update_position(-timeInMs);//to have a negative distance
		return 0;
	}
	//Calculating when the car must stop
	gettimeofday(&now,NULL);
	absDateToStop.tv_sec = time(NULL) + timeInMs / 1000;
	absDateToStop.tv_nsec = now.tv_usec * 1000 + 1000 * 1000 * (timeInMs % 1000);
	absDateToStop.tv_sec += absDateToStop.tv_nsec / (1000 * 1000 * 1000);
	absDateToStop.tv_nsec %= (1000 * 1000 *1000);
	int stopReason;
	if(pthread_create(&sensorsThread, NULL, &readingSensors, NULL))
	{
		printf("Failed creating thread to detect obstacle\n");
		return(2);
	}
	gettimeofday(&startDate,NULL);
	run_forever(speed,speed);
	if(inf==0) {
		stopReason = pthread_cond_timedwait(&obstacleDetected, &myMutex, &absDateToStop);
		gettimeofday(&stopDate,NULL);
		//For optimisation purposes the thread checking for obstacle will stop the car if obstacle is detected
		stop_car(); // Make sure the car is stopped
	}
	else {
		stopReason = pthread_cond_wait(&obstacleDetected, &myMutex);
		gettimeofday(&stopDate,NULL);
	}
	elapsedTime =(stopDate.tv_sec*1000 + stopDate.tv_usec/1000) - (startDate.tv_sec*1000 + startDate.tv_usec/1000);
	update_position(elapsedTime);
	pthread_mutex_unlock(&myMutex); //Obstacle Detected if(inf){ return 1; }
	if(stopReason == 0) return elapsedTime;
	else return 0; //TIMEDOUT 
}
