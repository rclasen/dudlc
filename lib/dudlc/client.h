#ifndef _MSERVCLIENT_CLIENT_H
#define _MSERVCLIENT_CLIENT_H

#include "session.h"
#include "iterate.h"


// TODO: make addr a number
typedef struct {
	int id;
	int uid;
	char *addr;
} msc_client;

void msc_client_free( msc_client *c );

#define msc_it_client	_msc_iter
#define msc_it_client_cur(x)	(msc_client*)_msc_it_cur((_msc_iter*)x)
#define msc_it_client_next(x)	(msc_client*)_msc_it_next((_msc_iter*)x)
#define msc_it_client_done(x)	_msc_it_done((_msc_iter*)x)

msc_it_client *msc_cmd_clientlist( mservclient *p );

int msc_cmd_clientcloseuser( mservclient *p, int uid );

int msc_cmd_clientclose( mservclient *p, int id );

void _msc_bcast_client( mservclient *c, const char *line );

#endif

