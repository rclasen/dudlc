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
#include "dudlc/sfilter.h"

duc_it_sfilter *_duc_it_sfilter_new( dudlc *p, const char *cmd, ... )
{
	_duc_iter *it;
	va_list ap;

	va_start(ap,cmd);
	it = _duc_it_newv(p, 
		(_duc_converter)_duc_sfilter_parse,
		(_duc_free_func)duc_sfilter_free,
		cmd, ap );
	va_end(ap);

	return it;
}

duc_sfilter *_duc_sfilter_parse( char *line, char **end )
{
	duc_sfilter *n;
	char *s;
	char *e;

	e = s = line;

	if( NULL == (n = malloc(sizeof(duc_sfilter))))
		goto clean1;
	memset( n, 0, sizeof(duc_sfilter) );

	n->id = strtol( s, &e, 10 );
	if( s == e )
		goto clean2;

	s = e+1;
	if( NULL == ( n->name = _duc_fielddup(s, &e)))
		goto clean2;

	s = e+1;
	if( NULL == ( n->filter = _duc_fielddup(s, &e)))
		goto clean3;


	if( end ) *end = e;
	return n;

clean3:
	free(n->name);
clean2:
	free(n);
clean1:
	if( end ) *end = line;
	return NULL;
}

void duc_sfilter_free( duc_sfilter *n )
{
	if( ! n )
		return;
	free(n->name);
	free(n->filter);
	free(n);
}

duc_sfilter *duc_cmd_sfilterget( dudlc *c, int id )
{
	return _duc_cmd_conv(c, (_duc_converter)_duc_sfilter_parse, 
			"sfilterget %d", id );
}

int duc_cmd_sfilter2id( dudlc *c, const char *name )
{
	return _duc_cmd_int(c, "sfilter2id %s", name );
}


duc_it_sfilter *duc_cmd_sfilterlist( dudlc *c )
{
	return _duc_it_sfilter_new(c, "sfilterlist" );
}

int duc_cmd_sfilteradd( dudlc *c, const char *name )
{
	return _duc_cmd_int(c, "sfilteradd %s", name );
}

int duc_cmd_sfiltersetname( dudlc *c, int id, const char *name )
{
	return _duc_cmd_succ( c, "sfiltersetname %d %s", id, name );
}

int duc_cmd_sfiltersetfilter( dudlc *c, int id, const char *filter )
{
	return _duc_cmd_succ( c, "sfiltersetfilter %d %s", id, filter );
}

int duc_cmd_sfilterdel( dudlc *c, int id )
{
	return _duc_cmd_succ( c, "sfilterdel %d", id );
}

