void run_forever( int l_speed, int r_speed );

void rotate_car(float angle,char D,int speed_circular);

void rotate_car2(int angle, char D, int speed_circular);

int is_running( void );

void run_timed( int l_speed, int r_speed, int ms );

void run_to_rel_pos( int l_speed, int l_pos, int r_speed, int r_pos );

void stop_car( void );

int init_motors();

void moveinf(int speed,char D);

int move(int speed, int time, int inf, char D);


void open_servo();

void close_servo();

void catch_object();

void servo (int speed, int angle);

void release();
