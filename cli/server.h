
#define BUFLENRCV	2048

typedef struct {
	char *host;
	int port;

	int fd;

	char buf[BUFLENRCV];
	int ilen;
	int llen;
} t_server;

t_server *server_open( const char *host, int port );
void server_close( t_server *s );

int server_connect( t_server *s );
void server_disconnect( t_server *s );
int server_reconnect( t_server *s );

int server_fd( t_server *t );

int server_recv( t_server *s );
int server_send( t_server *s, const char *cmd );
char *server_getline( t_server *s );


