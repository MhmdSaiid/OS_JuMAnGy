#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#ifndef BT
#define BT
#include "bluetoothFunctions.h"
#endif

#define Sleep(msec) usleep ((msec)*1000)
extern float x_position;
extern float y_position;
extern int socket_number;

void *sending_process(void) {
        printf("Hello from the thread\n");
	//socket_number = init_bluetooth_game();
	//printf("Socket created \n");

        while (1) {
                /*while (pthread_mutex_lock(&locker) != 0) {
                        printf("Values locked, sending thread will wait a little bit\n");
                        sleep(50);
                }*/
                printf("THREAD Sending position: X=%f, Y=%f\n", x_position, y_position);
               	send_position(socket_number,x_position,y_position);
		 //Put here the call to the sendposition function
                //pthread_mutex_unlock(&locker);
                Sleep(1700);
        }
}

