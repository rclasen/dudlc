/*
 * Copyright (c) 2008 Rainer Clasen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

#ifndef _DUDLC_SESSION_H
#define _DUDLC_SESSION_H

#include "sock.h"


typedef struct {
	void *ev;

	char *user;
	char *pass;

	t_duc_sock *con;
	int vmajor;
	int vminor;

	int inreply;
	char code[4];
	char *line;
} dudlc;


dudlc *duc_new( const char *hostname, int port);
void duc_free( dudlc *p );

int duc_setauth( dudlc *c, const char *user, const char *pass );
int duc_open( dudlc *p );
void duc_close( dudlc *p );

int duc_fd( dudlc *p );
void duc_poll( dudlc *p );

int duc_connected( dudlc *p );

int duc_cmd_auth( dudlc *p );

#endif
