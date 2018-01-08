#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "linkedlist.h"

float TRESHROTATION = 2.5;		//cms
int XROBOT;		//cms
int YROBOT;		//cms
int XMAX;
int YMAX;

uint8_t * map;
typedef struct position {
	int x;
	int y;
	uint8_t type;
	struct position * next;
} position_t;


typedef struct boundary {
	int x;
	int y;
	struct boundary * next;
} boundary_t;


position_t * push_to_last(position_t * last, int x, int y, uint8_t type) {
    last->next = malloc(sizeof(position_t));
    if (last->next == NULL) {
    	return(last);
    }
    last->next->x = x;
    last->next->y = y;
    last->next->type = type;
    last->next->next = NULL;
    return(last->next);
}




void push_to_first(position_t ** head, int x, int y, uint8_t type) {
	position_t * new_head;
	new_head = malloc(sizeof(position_t));
	new_head -> x = x;
	new_head -> y = y;
	new_head -> type = type;
	new_head -> next = *head;
	*head = new_head;
}

void push_bound_to_first(boundary_t ** head, int x, int y) {
	//Gets x and y, the coordinates as cases (so 5 time less than cms)
	boundary_t * new_head;
	new_head = malloc(sizeof(boundary_t));
	new_head -> x = x;
	new_head -> y = y;
	new_head -> next = *head;
	*head = new_head;
}


void print_pos_list(position_t * head) {
    position_t * current = head;

    while (current != NULL) {
        printf("%d, %d, %d\n", current->x, current->y, current->type);
        current = current->next;
    }
}

void print_bound_list(boundary_t * head) {
    boundary_t * current = head;

    while (current != NULL) {
        printf("%d, %d\n", current->x, current->y);
        current = current->next;
    }
}

position_t * initialize(int x, int y) {
	position_t * linkedList = NULL;
	linkedList = malloc(sizeof(position_t));
	if(linkedList == NULL) {
		return NULL;
	}
	push_to_first(&linkedList, x, y, 1);
	return(linkedList);
}




boundary_t * get_issuing_boundaries(boundary_t * obstacles) {
	//Not sufficient, we need a "get issuing obstacles" too
	boundary_t * current = obstacles;
	boundary_t * issuing_boundaries = NULL;
	while (current != NULL) {
		int x = current->x;
		int y = current->y;
		if( (XROBOT-x*5)*(XROBOT-x*5) + (YROBOT-y*5)*(YROBOT-y*5) <= TRESHROTATION*TRESHROTATION ) {
			push_bound_to_first(&issuing_boundaries, x, y);
		}
		current = current->next;
	}
	return(issuing_boundaries);
}

uint8_t* initializeMap(boundarie_t * obstacles, int xmax, int ymax) {
	size_t size = sizeof(uint8_t);
	uint8_t* map = malloc(size*xmax*ymax);
	boundary_t * current = obstacles;
	while(current!=NULL) {
		int x = current->x;
		int y = current->y;
		map[x*ymax*size+y] = 4;
		current = current->next;
	}
	for (int i=0; i<xmax; i++){
		for (int j=0; j<ymax; j++) {
			if (map[i*ymax*size+j] != 4) {
				map[i*ymax*size+j] = 0;
			}
		}
	}
	return(map);
}

void print_map(uint8_t * map, int xmax, int ymax) {
	for (int y=0; y<ymax; y++) {
		for(int x=0; x<xmax; x++) {
			printf("%u", map[x*ymax*sizeof(uint8_t)+y]);
		}
		printf("\n");
	}
	printf("\n\n");
}

uint8_t getFromMap(uint8_t * map, int x, int y) {
	//Returns the element of the map at position x,y
	return(map[x*YMAX*sizeof(uint8_t)+y]);
}

void setOnMap(uint8_t * map, int x, int y, uint8_t type) {
	//Sets the element of the map at x,y to type value, or returns "out of bounds" if impossible
	if (x>0 && x< XMAX && y>0 && y<YMAX) {
		map[x*YMAX*sizeof(uint8_t)+y] = type;
	} else {
		printf("out of bound\n");
	}
}


void main() {
	position_t * linkedList = initialize(5, 5);
	position_t * last = linkedList;
	uint8_t type = 1;
	int x = 5;
	int y = 5;
	/*for(int i = 0; i < 15; i++){
		x += 3;
		y += 4;
		last = push_to_last(last, x, y, type);
	}
	print_pos_list(linkedList);
	*/
	boundary_t * boundariesList = NULL;
	push_bound_to_first(&boundariesList, 1, 10);
	push_bound_to_first(&boundariesList, 1, 9);
	push_bound_to_first(&boundariesList, 1, 8);
	push_bound_to_first(&boundariesList, 2, 7);
	push_bound_to_first(&boundariesList, 2, 6);
	push_bound_to_first(&boundariesList, 2, 5);
	push_bound_to_first(&boundariesList, 2, 4);
	push_bound_to_first(&boundariesList, 2, 3);
	push_bound_to_first(&boundariesList, 2, 2);
	push_bound_to_first(&boundariesList, 2, 1);
	push_bound_to_first(&boundariesList, 2, 0);
	push_bound_to_first(&boundariesList, 20, 0);
	push_bound_to_first(&boundariesList, 15, 10);
	XROBOT = 4;
	YROBOT = 4;
	print_bound_list(get_issuing_boundaries(boundariesList));

	int xmax = 0;
	int ymax = 0;
	boundarie_t * current = boundariesList;
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
	printf("%d, %d\n", xmax, ymax);
	map = initializeMap(boundariesList, xmax, ymax);
	print_map(map, xmax, ymax);
	printf("%u\n\n", getFromMap(map, 3,2));
	setOnMap(map, 13, 10, 1);
	print_map(map, xmax, ymax);
}
