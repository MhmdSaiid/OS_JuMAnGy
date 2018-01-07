#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

float TRESHROTATION = 2.5;
int XROBOT;
int YROBOT;


typedef struct position {
	int x;
	int y;
	uint8_t type;
	struct position * next;
} position_t;


typedef struct boundarie {
	int x;
	int y;
	struct boundarie * next;
} boundarie_t;


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

void push_bound_to_first(boundarie_t ** head, int x, int y) {
	boundarie_t * new_head;
	new_head = malloc(sizeof(boundarie_t));
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

void print_bound_list(boundarie_t * head) {
    boundarie_t * current = head;

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




boundarie_t * get_issuing_boundaries(boundarie_t * obstacles) {
	boundarie_t * current = obstacles;
	boundarie_t * issuing_boundaries = NULL;
	while (current != NULL) {
		int x = current->x;
		int y = current->y;
		if( (XROBOT-x)*(XROBOT-x) + (YROBOT-y)*(YROBOT-y) <= TRESHROTATION*TRESHROTATION ) {
			push_bound_to_first(&issuing_boundaries, x, y);
		}
		current = current->next;
	}
	return(issuing_boundaries);
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
	boundarie_t * boundariesList = NULL;
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

}