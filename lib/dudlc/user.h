/*
 * Copyright (c) 2008 Rainer Clasen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

#ifndef _DUDLC_USER_H
#define _DUDLC_USER_H

#include "session.h"
#include "iterate.h"

typedef struct {
	int id;
	char *name;
	int right;
} duc_user;

duc_user *_duc_user_parse( char *line, char **end );
void duc_user_free( duc_user *u );
DEFINE_DUC_ITER_PARSE(user);

duc_it_user *duc_cmd_userlist( dudlc *c );
duc_user *duc_cmd_userget( dudlc *c, int uid );
int duc_cmd_usergetname( dudlc *c, const char *name );
int duc_cmd_usersetpass( dudlc *c, int uid, const char *pass );
int duc_cmd_usersetright( dudlc *c, int uid, int right );
int duc_cmd_useradd( dudlc *c, const char *name );
int duc_cmd_userdel( dudlc *c, int uid);



#endif
