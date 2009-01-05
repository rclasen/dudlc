/*
 * Copyright (c) 2008 Rainer Clasen
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

#include <stdlib.h>
#include <string.h>

#include "dudlc/proto.h"
#include "dudlc/event.h"
#include "dudlc/history.h"

duc_it_history *_duc_it_history_new( dudlc *p, const char *cmd, ... )
{
	_duc_iter *it;
	va_list ap;

	va_start(ap,cmd);
	it = _duc_it_newv(p, 
		(_duc_converter)_duc_history_parse,
		(_duc_free_func)duc_history_free,
		cmd, ap );
	va_end(ap);

	return it;
}

duc_history *_duc_history_parse( char *line, char **end )
{
	duc_history *n;
	char *s;
	char *e;

	e = s = line;

	if( NULL == (n = malloc(sizeof(duc_history))))
		goto clean1;
	memset( n, 0, sizeof(duc_history) );

	n->played = strtol( s, &e, 10 );
	if( s == e )
		goto clean2;

	s = e+1;
	if( NULL == ( n->user = _duc_user_parse( s, &e )))
		goto clean2;

	s = e+1;
	if( NULL == ( n->track = _duc_track_parse( s, &e )))
		goto clean3;


	if( end ) *end = e;
	return n;

clean3:
	duc_user_free(n->user);
clean2:
	free(n);
clean1:
	if( end ) *end = line;
	return NULL;
}

void duc_history_free( duc_history *q )
{
	if( ! q )
		return;
	duc_user_free(q->user);
	duc_track_free(q->track);
	free(q);
}

duc_it_history *duc_cmd_history( dudlc *c, int num )
{
	return _duc_it_history_new( c, "history %d", num );
}

duc_it_history *duc_cmd_historytrack( dudlc *c, int tid, int num )
{
	return _duc_it_history_new( c, "historytrack %d %d", tid, num );
}



