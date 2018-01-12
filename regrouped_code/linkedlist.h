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

position_t * push_to_last(position_t * last, int x, int y, uint8_t type);

void push_to_first(position_t ** head, int x, int y, uint8_t type) ;

void push_bound_to_first(boundary_t ** head, int x, int y);

void add_bound_line(boundary_t ** head, float start_x, float start_y,float new_x,float new_y);

void print_pos_list(position_t * head);

void print_bound_list(boundary_t * head);

void getSize(boundary_t * boundaries);

position_t * initialize(int x, int y) ;

boundary_t * get_issuing_boundaries(boundary_t * obstacles);

uint8_t* initializeMap(boundary_t * obstacles);

void print_map(uint8_t * map);

uint8_t getFromMap(uint8_t * map, int x, int y);

void setOnMap(uint8_t * map, int x, int y, uint8_t type);

void addLineOf(uint8_t * map, int xbeg, int ybeg, int xend, int yend, uint8_t type);

uint8_t * small_stadium_map(int x_dimension, int y_dimension);
