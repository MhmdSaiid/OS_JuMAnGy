#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <time.h>
#include <sys/socket.h>
#include <math.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include "bluetoothFunctions.h"
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
#define Sleep( msec ) usleep(( msec ) * 1000 )

uint16_t msgId = 0;
extern int XMAX;
extern int YMAX;

int read_from_server (int sock, char *buffer, size_t maxSize) {
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
  //char type;
	char string[58];
	int x_send= (int)ceil(x/5);
	int y_send = (int)ceil(y/5);

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

void send_map(int socket, uint8_t * map){
  uint8_t mapPixel;
  *((uint16_t *) string) = msgId++;
  int x;
  int y;
  for (x=0; x<XMAX; x++){
		for (y=0; y<YMAX; y++) {
			mapPixel = getFromMap(map,x,y);
      string[2] = TEAM_ID;
      string[3] = 0xFF;
      string[4] = MSG_MAPDATA;
      string[5] = x;          /* x */
      string[6] = 0x00;
      string[7] = y;             /* y */
      string[8]= 0x00;
      string[9]= mapPixel*20;
      string[10]=mapPixel*20;
      string[11]= mapPixel*20;
      write(socket, string, 12);
		}
	}
}
int init_bluetooth_game()
{
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

   }
   else {
      fprintf (stderr, "Failed to connect to server...\n");
      sleep (2);
      exit (EXIT_FAILURE);

   }
  return s;
}

void close_comm(int s)
{
  close(s);
}

/*
void debug (const char *fmt, ...) {
  va_list argp;

  va_start (argp, fmt);

  vprintf (fmt, argp);

  va_end (argp);
}


int s;

uint16_t msgId = 0;



void robot () {
  char string[58];
  char type;
  int x1, x2, y1, y2;
  printf ("I'm navigating...\n");

  printf ("I'm sending my map...\n");

  /* MAP data is in the form MAPDATA | X  X |Y  Y | R | G | B

  /* Create 1 square green obstacle and 1 round red obstacle
  /* Send only the outline
  x1 = rand() % 30;
  y1= rand() % 30;

  for (i=x1; i<x1+10; i++){
    *((uint16_t *) string) = msgId++;
    string[2] = TEAM_ID;
    string[3] = 0xFF;
    string[4] = MSG_MAPDATA;
    string[5] = i;          /* x
    string[6] = 0x00;
    string[7] = y1;             /* y
    string[8]= 0x00;
    string[9]= 0;
    string[10]=254;
    string[11]= 0;
    write(s, string, 12);
    Sleep( 100 );
  }
  for (i=x1; i<x1+10; i++){
    *((uint16_t *) string) = msgId++;
    string[2] = TEAM_ID;
    string[3] = 0xFF;
    string[4] = MSG_MAPDATA;
    string[5] = i;          /* x
    string[6] = 0x00;
    string[7] = y1+10;          /* y
    string[8]= 0x00;
    string[9]= 0;
    string[10]=254;
    string[11]= 0;
    write(s, string, 12);
    Sleep( 100 );
  }
  for (j=y1; j<y1+10; j++){
    *((uint16_t *) string) = msgId++;
    string[2] = TEAM_ID;
    string[3] = 0xFF;
    string[4] = MSG_MAPDATA;
    string[5] = x1;          /* x
    string[6] = 0x00;
    string[7] = j;              /* y
    string[8]= 0x00;
    string[9]= 0;
    string[10]=254;
    string[11]= 0;
    write(s, string, 12);
    Sleep( 100 );
  }

  for (j=y1; j<y1+10; j++){
    *((uint16_t *) string) = msgId++;
    string[2] = TEAM_ID;
    string[3] = 0xFF;
    string[4] = MSG_MAPDATA;
    string[5] = x1+10;          /* x
    string[6] = 0x00;
    string[7] = j;              /* y
    string[8]= 0x00;
    string[9]= 0;
    string[10]=254;
    string[11]= 0;
    write(s, string, 12);
    Sleep( 100 );
  }

  x2 = 15 +rand() % 20;
  y2= 15 + rand() % 20;


  for (i=x2-15; i<x2+15; i++){
    for (j=y2-15; j<y2+15; j++){
      if (sqrt((i-x2)*(i-x2) + (j-y2)*(j-y2)) < 15 && sqrt((i-x2)*(i-x2) + (j-y2)*(j-y2))>14){
        *((uint16_t *) string) = msgId++;
        string[2] = TEAM_ID;
        string[3] = 0xFF;
        string[4] = MSG_MAPDATA;
        string[5] = i;          /* x
        string[6] = 0x00;
        string[7] = j;          /* y
        string[8]= 0x00;
        string[9]= 254;
        string[10]=0;
        string[11]= 0;
        write(s, string, 12);
        Sleep( 100 );
      }
    }
  }
  printf("Done sending map");
  *((uint16_t *) string) = msgId++;
  string[2] = TEAM_ID;
  string[3] = 0xFF;
  string[4] = MSG_MAPDONE;
  write(s, string, 5);

  printf("I'm waiting for the stop message");
  while(1){
    //Wait for stop message
    read_from_server (s, string, 58);
    type = string[4];
    if (type ==MSG_STOP){
      return;
    }
  }
}
*/

/*
int main(int argc, char **argv) {
  struct sockaddr_rc addr = { 0 };
  int status;

  /* allocate a socket
  s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

  /* set the connection parameters (who to connect to)
  addr.rc_family = AF_BLUETOOTH;
  addr.rc_channel = (uint8_t) 1;
  str2ba (SERV_ADDR, &addr.rc_bdaddr);

  /* connect to server
  status = connect(s, (struct sockaddr *)&addr, sizeof(addr));

  /* if connected
  if( status == 0 ) {
    char string[58];

    /* Wait for START message
    read_from_server (s, string, 9);
    if (string[4] == MSG_START) {
      printf ("Received start message!\n");


    }
    robot();
    close (s);

    sleep (5);

  } else {
    fprintf (stderr, "Failed to connect to server...\n");
    sleep (2);
    exit (EXIT_FAILURE);
  }

  close(s);
  return 0;
}

*/
