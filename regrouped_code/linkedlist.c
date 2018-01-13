#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "math.h"
#include "ev3.h"
#include "ev3_port.h"
#include "ev3_sensor.h"
#include "ev3_tacho.h"
#ifdef __WIN32__

#include <windows.h>

// UNIX //////////////////////////////////////////
#else
#include <unistd.h>
#endif


float THRESHROTATION = 8;		//mapcoordinates
float XROBOT;		//cms
float YROBOT;		//cms
int XMAX;		//mapcoordinates
int YMAX;		//mapcoordinates
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
extern float relative_angle;
extern float val;


typedef struct boundary {
	/*Written by Armand PERON*/
	//Type defined as a linkedList of position (x,y)
	//Intended to be used as boundary list
	int x;
	int y;
	struct boundary * next;
} boundary_t;



void push_bound_to_first(boundary_t ** head, int x, int y) {
	/*Written by Armand PERON*/
	//Adds a new point to the Boundaries linkedList
	//Gets x and y, the coordinates as cases (so 5 time less than cms)
	boundary_t * new_head;
	new_head = malloc(sizeof(boundary_t));
	new_head -> x = x;
	new_head -> y = y;
	new_head -> next = *head;
	*head = new_head;
}

void add_bound_line(boundary_t ** head, float xbeg, float ybeg, float xend, float yend) {
	/*Written by Armand PERON*/
	//Saves a list of position into the boundary list
	float dx = (xend - xbeg)/5;
	float dy = (yend - ybeg)/5;
	//printf("From %f, %f to %f, %f\n", xbeg, ybeg, xend, yend);
	int nb_blocks = (int) (sqrt(dx*dx + dy*dy));
	float x = (float) round(xbeg/5);
	float y = round(ybeg/5);
	int i;
	for (i = 0; i < nb_blocks; i++) {
		push_bound_to_first(head, round(x), round(y));
		x += dx/nb_blocks;
		y += dy/nb_blocks;
		//printf("%f, %f\n", x, y);
	}
}

void print_bound_list(boundary_t * head) {
    boundary_t * current = head;
    while (current != NULL) {
        printf("Position: (%d, %d)\n", current->x, current->y);
        current = current->next;
    }
}



void getSize(boundary_t * boundaries) {
	/*Written by Armand PERON*/
	//From a boundaries linked list, actualise the value of XMAX and YMAX
	//XMIN and YMIN are set to 0 and do not change their values
	int xmax = 0;
	int ymax = 0;
	boundary_t * current = boundaries;
	while (current != NULL) {
		if (current->x > xmax-1) {
			xmax = current->x+1;
		}
		if (current->y > ymax-1) {
			ymax = current->y+1;
		}
		current=current->next;
	}
	XMAX = xmax;
	YMAX = ymax;
}


uint8_t* initializeMap(boundary_t * obstacles) {
	/*Written by Armand PERON*/
	//Initialize the map from the Boundary linked list
	size_t size = sizeof(uint8_t);
	getSize(obstacles);
	int xmax = XMAX;
	int ymax = YMAX;
	uint8_t* map = malloc(size*xmax*ymax);
	boundary_t * current = obstacles;
	while(current!=NULL) {
		int x = current->x;
		int y = current->y;
		map[x*ymax*size+y] = 4;
		current = current->next;
	}
	int i;
	int j;
	for (i=0; i<xmax; i++){
		for (j=0; j<ymax; j++) {
			if (map[i*ymax*size+j] != 4) {
				map[i*ymax*size+j] = 0;
			}
		}
	}
	return(map);
}

void print_map(uint8_t * map) {
	/*Written by Armand PERON*/
	int xmax = XMAX;
	int ymax = YMAX;
	printf("xmax :%d, ymax : %d \n", xmax, ymax);
	int y;
	int x;
	for (y=0; y<ymax; y++) {
		for(x=0; x<xmax; x++) {
			printf("%u", map[x*ymax*sizeof(uint8_t)+y]);
		}
		printf("\n");
	}
	printf("\n\n");
}

uint8_t getFromMap(uint8_t * map, int x, int y) {
	/*Written by Armand PERON*/
	//Returns the element of the map at position x,y
	return(map[(int)(floor(x/5)*YMAX*sizeof(uint8_t)+floor(y/5))]);
}

void setOnMap(uint8_t * map, int x, int y, uint8_t type) {
	/*Written by Armand PERON*/
	//Sets the element of the map at x,y to type value, or ignore the command if (x,y) is out of bound
	if (x>0 && floor(x/5)< XMAX && y>0 && floor(y/5)<YMAX) {
		map[(int)(floor(x/5)*YMAX*sizeof(uint8_t)+floor(y/5))] = type;
	} else {
		//printf("out of bound: %d, %d\n", x, y);
	}
}

void add_line_of(uint8_t * map, int xbeg, int ybeg, int xend, int yend, uint8_t type) {
	/*Written by Armand PERON*/
	//Sets the values of all blocks between (xbeg, ybeg) and (xend, yend) to type
	//Gets positions in cms
	float dx = (xend - xbeg);
	float dy = (yend - ybeg);
	printf("dx = %f, dy = %f",dx,dy);
	int nb_blocks = (int) (sqrt(dx*dx + dy*dy));
	float x = xbeg;
	float y = ybeg;
	int i;
	for (i = 0; i < nb_blocks/5; i++) {
		setOnMap(map, x, y, type);
		x += 5*dx/nb_blocks;
		y += 5*dy/nb_blocks;
	}
}

void add_big_line_of(uint8_t * map, int xbeg, int ybeg, int xend, int yend, int width, uint8_t type) {
	/*Written by Armand PERON*/
	//Adds several lines using "add_line_of" function to create a rectangle of specified width
	//Gets position and width as cms
	float dx = (xend - xbeg);
	float dy = (yend - ybeg);
	float l = (float) sqrt(dx*dx + dy*dy);
	float widx = -dy/l;
	float widy = dx/l;
	//widx and widy defines a unitarian vector perpendicular to the line
	float x = xbeg - widx*width/2;
	float y = ybeg - widy*width/2;
	float area = 0; 		//Gets the value of the width actually checked
	while (area < width) {
		add_line_of(map, (int) x, (int) y, (int) round(x+dx), (int) round(y+dy), type);
		x += widx*2.5;
		y += widy*2.5;
		area += 2.5;
	}
}
/*
Old version
bool check_area_obstacle(uint8_t * map, int x_offset, int y_offset,int x_dimension ,int y_dimension,uint8_t obstacle_type){
	int xrel;
	int yrel;
	bool is_there = false;
	for(xrel = 0; xrel < x_dimension; xrel ++) {
		for (yrel = 0; yrel < y_dimension; yrel ++) {
			if (floor(x_offset/5) + xrel < XMAX && floor(y_offset/5) + yrel < YMAX) {
				if (getFromMap(map, x_offset + 5*xrel, y_offset + 5*yrel) == obstacle_type) {
					is_there = true;
					return(is_there);
				}
			}
		}
	}
	return(is_there);
}

*/
bool check_area_obstacle(uint8_t * map, int x_offset, int y_offset, int length, int width, uint8_t type) {
	/*Written by Armand PERON*/
	//Check the rectangular area set from (x_offset, y_offset) as center of closest side, with parameters length and width
	//Defines the orientation of the area from "relative_angle" value
	//Gets position, length and width as cms
	float dlx = cos(relative_angle*val);
	float dly = sin(relative_angle*val);
	printf("dl: %f, %f\n", dlx, dly);
	float l = 0;		//length checked
	float w = 0;		//Width checked on current l
	float x;
	float y;
	int i = - (int) floor(width/2);
	int j = 0;
	bool is_there = false;
	while(i <= width/2) {
		while(j <= length) {
			x = x_offset - i*dly + j*dlx;
			y = y_offset + i*dlx + j*dly;
			if (floor(x/5) < XMAX && floor(y/5) < YMAX) {
				if(getFromMap(map,(int) round(x), (int) round(y)) == type) {
					is_there = true;
					return(is_there);
				}
				//setOnMap(map, (int) round(x), (int) round(y), 5);		//Uncomment to get checked area as 5s on the map
			}
			j += 2.5;
		}
		j = 0;
		i += 2.5;
	}
	return(is_there);
}


boundary_t * get_issuing_obstacles(uint8_t * map) {
	/*Written by Armand PERON*/
	//Returns a linked list of all obstacles in a square from THRESH * THRESH (not a circle)
	//Checks robot position from global variables
	boundary_t * issuing_obstacles = NULL;
	int xmin = (int) MIN(round((XROBOT - THRESHROTATION) / 5), XMAX-1);
	int xmax = (int) MAX(round((XROBOT + THRESHROTATION) / 5), 0);
	int ymin = (int) MIN(round((YROBOT - THRESHROTATION) / 5), YMAX-1);
	int ymax = (int) MAX(round((YROBOT + THRESHROTATION) / 5), 0);
	//printf("%f, %f, %f, %f\n", (XROBOT - THRESHROTATION) / 5, (XROBOT + THRESHROTATION) / 5, (YROBOT - THRESHROTATION) / 5, (YROBOT + THRESHROTATION) / 5);
	//printf("x = %d to %d,  y = %d to %d\n", xmin, xmax, ymin, ymax);
	int i;
	int j;
	for (i = xmin; i <= xmax; i++) {
		for (j = ymin; j <= ymax; j++) {
			if (getFromMap(map, i*5, j*5) == 4) {
				push_bound_to_first(&issuing_obstacles, i, j);
			}
		}
	}
	return(issuing_obstacles);
}




uint8_t * small_stadium_map(int x_dimension, int y_dimension){
	/*Written by Armand PERON*/
	//Creates a rectangular map with parameters dimensions
	boundary_t * boundariesList = NULL;
	add_bound_line(&boundariesList, 0, 0, x_dimension, 0);
	add_bound_line(&boundariesList, x_dimension, 0, x_dimension, y_dimension);
	add_bound_line(&boundariesList, x_dimension, y_dimension, 0, y_dimension);
	add_bound_line(&boundariesList, 0, y_dimension, 0, 0);
	getSize(boundariesList);
	uint8_t * map = initializeMap(boundariesList);
	print_map(map);
	return map;
}



/*
void main() {
	uint8_t * map = small_stadium_map(100, 100);
	print_map(map);
	add_big_line_of(map, 30, 50, 70, 50, 10, 1);
	print_map(map);
	relative_angle = 0.78;
	printf("Result of checking: %d\n", check_area_obstacle(map, 65, 70, 45, 10, 4));
	print_map(map);
}
*/
