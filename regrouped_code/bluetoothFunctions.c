#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdint.h>
#include <time.h>
#include <sys/socket.h>
#include <math.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include "bluetoothFunctions.h"
#include "ev3.h"
#include "ev3_port.h"
#include "ev3_sensor.h"
#include "ev3_tacho.h"
//
#ifndef INITIALIZE
#define INITIALIZE
#include "defines.h"
#endif
//#define SERV_ADDR   "5C:51:4F:A9:25:76"     /* Whatever the address of the server is */
//#define SERV_ADDR   "dc:53:60:ad:61:90"     /* Whatever the address of the server is */
//#define SERV_ADDR   "54:27:1E:35:B8:D6"     /* Whatever the address of the server is */
#define SERV_ADDR "54:27:1E:35:B8:D6" //justine's computer
//#define SERV_ADDR "00:1A:7D:DA:71:06" //teachers'one
#define TEAM_ID     13                       /* Your team ID */

#define MSG_ACK     0
#define MSG_START    1
#define MSG_STOP   2
#define MSG_KICK    3
#define MSG_POSITION 4
#define MSG_MAPDATA  5
#define MSG_MAPDONE 6
#define OBSTACLE 7
#define Sleep( msec ) usleep(( msec ) * 1000 )

uint16_t msgId = 0;
extern int XMAX;
extern int YMAX;
extern int finished;

int read_from_server (int sock, char *buffer, size_t maxSize) {
  /*written by Gautier Dervaux*/
  int bytes_read = read (sock, buffer, maxSize);

  if (bytes_read <= 0) {
    fprintf (stderr, "Server unexpectedly closed connection...\n");
    close(sock);
    exit (EXIT_FAILURE);
  }

  printf ("[DEBUG] received %d bytes\n", bytes_read);

  return bytes_read;
}


void send_position(int s, float x,float y) {
	/* written by Gautier Dervaux*/

	char string[58];
	int x_send= (int)ceil(x/5);
	int y_send = (int)ceil(y/5);
	/*according to the protocol*/
	*((uint16_t *) string) = msgId++;
	string[2] = TEAM_ID;
	string[3] = 0xFF;
	string[4] = MSG_POSITION;
	string[5] = x_send;          /* x */
	string[6] = 0x00;
	string[7] = y_send;              /* y */
	string[8]= 0x00;
	write(s, string, 9);
}

void send_map(int s, uint8_t * map){
  /*written by Gautier Dervaux*/
  uint8_t mapPixel;
  char string[58];
  *((uint16_t *) string) = msgId++;
  int x;
  int y;
  int color;
  for (x=0; x<XMAX; x++){
	for (y=0; y<YMAX; y++) {
		mapPixel = getFromMap(map,5*x,5*y);
    if(mapPixel ==4)  color = 0; //black for boundary
    else color = 255;
      		if(mapPixel!=EMPTY){
			       string[2] = TEAM_ID;
       			 string[3] = 0xFF;
       			 string[4] = MSG_MAPDATA;
        		 string[5] = x;          /* x */
        		 string[6] = 0x00;
       			 string[7] = y;             /* y */
       			 string[8]= 0x00;
			       string[9] = 0;
       			 string[10]= 0;
       			 string[11]= color;
       			 write(s, string, 12);
             Sleep(100);
      		}
	}
  }

string[4] = MSG_MAPDONE;
write(s, string, 5);
}


void send_obstacle(int s, uint8_t * map,float x,float y,bool act){
/*written by Mohammed Saeed*/
char string[58];
*((uint16_t *) string) = msgId++;

string[2] = TEAM_ID;
string[3] = 0xFF;
string[4] = OBSTACLE;
string[5] = (char) act;          /* x */
string[6] = 0x00;
string[7] = x;             /* y */
string[8]= 0x00;
string[9] = y;

write(s, string, 10);



}

void* finish(void* arg)
{/*Written by Mohammed Saeed*/
Sleep(3*60*1000+30*1000);
finished=1;
//printf("FINISHED\n");
return NULL;
}



int init_bluetooth_game()
{
  /*written by Gautier Dervaux,thread initialisation by Mohammed Saeed*/
  char string[58];
  struct sockaddr_rc addr = { 0 };
  int status;
  int s;

  /* allocate a socket */
  s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

  /* set the connection parameters (who to connect to) */
  addr.rc_family = AF_BLUETOOTH;
  addr.rc_channel = (uint8_t) 1;
  str2ba (SERV_ADDR, &addr.rc_bdaddr);

  /* connect to server */
  status = connect(s, (struct sockaddr *)&addr, sizeof(addr));
  /* if connected */
  if( status == 0 ) {

    printf(" Connection established, waiting start message...\n");
    /* Wait for START message */
    read_from_server (s, string, 9);
    if (string[4] == MSG_START) printf ("Received start message!\n");
    pthread_t tid;
    int fini;
    fini=pthread_create(&tid,NULL,&finish,NULL);
    if(fini!=0)
    {printf("Thread didn't initialise\n");
     exit(EXIT_FAILURE);}

   }
   else {
      fprintf (stderr, "Failed to connect to server...\n");
      sleep (2);
      exit (EXIT_FAILURE);

   }
  return s;
}

void close_comm(int s)
/*written by Gautier Dervaux*/
{
  close(s);
}
