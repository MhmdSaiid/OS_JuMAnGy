int read_from_server (int sock, char *buffer, size_t maxSize);

void send_position(int s,float x,float y);

void send_map(int socket, uint8_t * map);

int init_bluetooth_game();

void close_comm(int s);
