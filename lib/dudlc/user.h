#ifndef _MSERVCLIENT_USER_H
#define _MSERVCLIENT_USER_H

#include <mservclient/session.h>
#include <mservclient/iterate.h>

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



msc_it_user *msc_cmd_userlist( mservclient *c );
msc_user *msc_cmd_userget( mservclient *c, int uid );
int msc_cmd_usergetname( mservclient *c, const char *name );
int msc_cmd_usersetpass( mservclient *c, int uid, const char *pass );
int msc_cmd_usersetright( mservclient *c, int uid, int right );
int msc_cmd_useradd( mservclient *c, const char *name );
int msc_cmd_userdel( mservclient *c, int uid);



#endif
