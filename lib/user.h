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



typedef struct {
	int id;
	char *name;
	int right;
} msc_user;

void msc_user_free( msc_user *u );

#define msc_it_user	_msc_iter
#define msc_it_user_cur(x)	(msc_user*)_msc_it_cur((_msc_iter*)x)
#define msc_it_user_next(x)	(msc_user*)_msc_it_next((_msc_iter*)x)
#define msc_it_user_done(x)	_msc_it_done((_msc_iter*)x)


int msc_cmd_kick( mservclient *p, int uid );

msc_it_user *msc_cmd_users( mservclient *c );
msc_user *msc_cmd_userget( mservclient *c, int uid );
int msc_cmd_usergetname( mservclient *c, const char *name );
int msc_cmd_usersetpass( mservclient *c, int uid, const char *pass );
int msc_cmd_usersetright( mservclient *c, int uid, int right );
int msc_cmd_useradd( mservclient *c, const char *name );
int msc_cmd_userdel( mservclient *c, int uid);


void _msc_bcast_user( mservclient *c, const char *line );

#endif
