#ifndef _DUDLC_USER_H
#define _DUDLC_USER_H

#include "session.h"
#include "iterate.h"

typedef struct {
	int id;
	char *name;
	int right;
} duc_user;

duc_user *_duc_user_parse( const char *line, char **end );
void duc_user_free( duc_user *u );

#define duc_it_user	_duc_iter
#define duc_it_user_cur(x)	(duc_user*)_duc_it_cur((_duc_iter*)x)
#define duc_it_user_next(x)	(duc_user*)_duc_it_next((_duc_iter*)x)
#define duc_it_user_done(x)	_duc_it_done((_duc_iter*)x)



duc_it_user *duc_cmd_userlist( dudlc *c );
duc_user *duc_cmd_userget( dudlc *c, int uid );
int duc_cmd_usergetname( dudlc *c, const char *name );
int duc_cmd_usersetpass( dudlc *c, int uid, const char *pass );
int duc_cmd_usersetright( dudlc *c, int uid, int right );
int duc_cmd_useradd( dudlc *c, const char *name );
int duc_cmd_userdel( dudlc *c, int uid);



#endif
