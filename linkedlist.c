#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct position {
	int x;
	int y;
	uint8_t type;
	struct position * next;
} position_t;



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

void print_list(position_t * head) {
    position_t * current = head;

    while (current != NULL) {
        printf("%d, %d, %d\n", current->x, current->y, current->type);
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

void main() {
	position_t * linkedList = initialize(5, 5);
	position_t * last = linkedList;
	uint8_t type = 1;
	int x = 5;
	int y = 5;
	for(int i = 0; i < 15; i++){
		x += 3;
		y += 4;
		last = push_to_last(last, x, y, type);
	}
	print_list(linkedList);
}