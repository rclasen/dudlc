#ifndef _MSERVCLIENT_USER_H
#define _MSERVCLIENT_USER_H

#include <mservclient/client.h>
#include <mservclient/iterate.h>

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

msc_it_client *msc_cmd_who( mservclient *p );
int msc_cmd_kick( mservclient *p, int uid );

void _msc_bcast_user( mservclient *c, const char *line );

// TODO: remaining user/client commands

#endif
