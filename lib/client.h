#ifndef _MSERVCLIENT_PROTO_H
#define _MSERVCLIENT_PROTO_H

#include <mservclient/sock.h>

typedef struct {
	char *user;
	char *pass;

	t_msc_sock *con;
	int inreply;
	char code[4];
	const char *line;
} t_mservclient;

typedef void (*t_proto_bcast_msg)(const char *c);
typedef void (*t_proto_func_disconnect)( t_mservclient *p );

extern t_proto_func_disconnect cb_disconnect;
extern t_proto_bcast_msg cb_bcast;

t_mservclient *msc_new( const char *hostname, int port, 
		const char *user, const char *pass );
void msc_free( t_mservclient *p );
int msc_open( t_mservclient *p );

int msc_fd( t_mservclient *p );
void msc_poll( t_mservclient *p );

int _msc_send( t_mservclient *p, const char *msg );
int _msc_fsend( t_mservclient *p, const char *fmt, ... );

int _msc_last( t_mservclient *p );
int _msc_nextline( t_mservclient *p );
int _msc_rend( t_mservclient *p );
const char *_msc_rcode( t_mservclient *p );
const char *_msc_rline( t_mservclient *p );

#endif
