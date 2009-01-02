/*
 * Copyright (c) 2008 Rainer Clasen
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

#ifndef _DUDLC_CLIENT_H
#define _DUDLC_CLIENT_H

#include "session.h"
#include "iterate.h"
#include "user.h"


// TODO: make addr a number
typedef struct {
	int id;
	duc_user *user;
	char *addr;
} duc_client;

void duc_client_free( duc_client *c );

#define duc_it_client	_duc_iter
#define duc_it_client_cur(x)	(duc_client*)_duc_it_cur((_duc_iter*)x)
#define duc_it_client_next(x)	(duc_client*)_duc_it_next((_duc_iter*)x)
#define duc_it_client_done(x)	_duc_it_done((_duc_iter*)x)

duc_it_client *duc_cmd_clientlist( dudlc *p );

int duc_cmd_clientcloseuser( dudlc *p, int uid );

int duc_cmd_clientclose( dudlc *p, int id );

void _duc_bcast_client( dudlc *c, char *line );

#endif

