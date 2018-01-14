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
	/*written by Armand Peron*/
        printf("Hello from the thread\n");


        while (1) {

                printf("THREAD Sending position: X=%f, Y=%f\n", x_position, y_position);
               	send_position(socket_number,x_position,y_position);
                Sleep(1700);
        }
}

