#ifndef _PROTO_H
#define _PROTO_H

#include "server.h"

typedef struct {
	char *user;
	char *pass;

	t_server *con;
	int inreply;
	char code[4];
	const char *line;
} t_proto;

typedef void (*t_proto_func_disconnect)( t_proto *p );

extern t_proto_func_disconnect cb_disconnect;

t_proto *proto_new( const char *hostname, int port, 
		const char *user, const char *pass );
void proto_free( t_proto *p );
int proto_open( t_proto *p );

int proto_fd( t_proto *p );
void proto_poll( t_proto *p );

int proto_send( t_proto *p, const char *msg );
int proto_fsend( t_proto *p, const char *fmt, ... );

int proto_last( t_proto *p );
int proto_nextline( t_proto *p );
int proto_rend( t_proto *p );
const char *proto_rcode( t_proto *p );
const char *proto_rline( t_proto *p );

#endif
