#ifndef _MSERVCLIENT_CLIENT_H
#define _MSERVCLIENT_CLIENT_H

#include <mservclient/sock.h>



typedef struct {
	void *ev;

	char *user;
	char *pass;

	t_msc_sock *con;

	int inreply;
	char code[4];
	const char *line;
} mservclient;


mservclient *msc_new( const char *hostname, int port, 
		const char *user, const char *pass );
void msc_free( mservclient *p );

int msc_open( mservclient *p );
void msc_close( mservclient *p );

int msc_fd( mservclient *p );
void msc_poll( mservclient *p );

const char *msc_rmsg( mservclient *p );

int msc_cmd_disconnect( mservclient *p, int id );

#endif

