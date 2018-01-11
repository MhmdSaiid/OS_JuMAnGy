#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

float THRESHROTATION = 8;		//mapcoordinates
float XROBOT;		//cms
float YROBOT;		//cms
int XMAX;		//mapcoordinates
int YMAX;		//mapcoordinates
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))


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

void add_bound_line(boundary_t ** head, float xbeg, float ybeg, float xend, float yend) {
	//saves a list of blocks into the boudary list
	float dx = xend - xbeg;
	float dy = yend - ybeg;
	printf("From %f, %f to %f, %f\n", xbeg, ybeg, xend, yend);
	int nb_blocks = (int) (sqrt(dx*dx + dy*dy));
	float x = (float) round(xbeg);
	float y = round(ybeg);
	int i;
	for (i = 0; i < nb_blocks; i++) {
		push_bound_to_first(head, round(x), round(y));
		x += dx/nb_blocks;
		y += dy/nb_blocks;
		printf("%f, %f\n", x, y);
	}
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




/*boundary_t * get_issuing_boundaries(boundary_t * obstacles) {
	//Not sufficient, we need a "get issuing obstacles" too
	boundary_t * current = obstacles;
	boundary_t * issuing_boundaries = NULL;
	while (current != NULL) {
		int x = current->x;
		int y = current->y;
		if( (XROBOT-x*5)*(XROBOT-x*5) + (YROBOT-y*5)*(YROBOT-y*5) <= THRESHROTATION*THRESHROTATION ) {
			push_bound_to_first(&issuing_boundaries, x, y);
		}
		current = current->next;
	}
	return(issuing_boundaries);
}
*/


void getSize(boundary_t * boundaries) {
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
	size_t size = sizeof(uint8_t);
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
	int xmax = XMAX;
	int ymax = YMAX;
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
	//Returns the element of the map at position x,y
	return(map[x*YMAX*sizeof(uint8_t)+y]);
}

void setOnMap(uint8_t * map, int x, int y, uint8_t type) {
	//Sets the element of the map at x,y to type value, or returns "out of bounds" if impossible
	if (x>0 && x< XMAX && y>0 && y<YMAX) {
		map[(int)((floor(x/5)*YMAX*sizeof(uint8_t)+floor(y/5)))] = type;
	} else {
		printf("out of bound: %d, %d\n", x, y);
	}
}

boundary_t * get_issuing_obstacles(uint8_t * map) {
	//Gets robot's position (in CMs, care) and returns a linked list of obstacles in a square from THRESH * THRESH (not a circle)

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
			if (getFromMap(map, i, j) == 4) {
				push_bound_to_first(&issuing_obstacles, i, j);
			}
		}
	}
	return(issuing_obstacles);
}
/*
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

	add_bound_line(&boundariesList, 15, 10, 20, 0);

	int xmax = 0;
	int ymax = 0;
	boundary_t * current = boundariesList;
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

	XROBOT = 20;
	YROBOT = 20;
	uint8_t * map = initializeMap(boundariesList);
	print_map(map);
	printf("issuing obstacles:\n");
	print_bound_list(get_issuing_obstacles(map));
}*/
