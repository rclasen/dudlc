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
#include "dudlc/artist.h"

duc_it_artist *_duc_it_artist_new( dudlc *p, const char *cmd, ... )
{
	_duc_iter *it;
	va_list ap;

	va_start(ap,cmd);
	it = _duc_it_newv(p,
		(_duc_converter)_duc_artist_parse,
		(_duc_free_func)duc_artist_free,
		cmd, ap );
	va_end(ap);

	return it;
}

duc_artist *_duc_artist_parse( char *line, char **end )
{
	duc_artist *n;
	char *s;
	char *e;

	e = s = line;

	if( NULL == (n = malloc(sizeof(duc_artist))))
		goto clean1;
	memset( n, 0, sizeof(duc_artist) );

	n->id = strtol( s, &e, 10 );
	if( s == e )
		goto clean2;

	s = e+1;
	if( NULL == ( n->artist = _duc_fielddup( s, &e )))
		goto clean2;

	if( end ) *end = e;
	return n;

clean2:
	free(n);
clean1:
	if( end ) *end = line;
	return NULL;
}

void duc_artist_free( duc_artist *a )
{
	if( ! a )
		return;
	free(a->artist);
	free(a);
}

duc_artist *duc_cmd_artistget( dudlc *c, int id )
{
	return _duc_cmd_conv( c, (_duc_converter)_duc_artist_parse,
			"artistget %d", id );
}

duc_it_artist *duc_cmd_artistlist( dudlc *c )
{
	return _duc_it_artist_new( c, "artistlist" );
}

duc_it_artist *duc_cmd_artiststag( dudlc *c, int tid )
{
	return _duc_it_artist_new( c, "artiststag %d", tid );
}

duc_it_artist *duc_cmd_artistsearch( dudlc *c, const char *str )
{
	return _duc_it_artist_new( c, "artistsearch %s", str );
}

int duc_cmd_artistsetname( dudlc *c, int id, const char *name )
{
	return _duc_cmd_succ(c, "artistsetname %d %s", id, name );
}

int duc_cmd_artistmerge( dudlc *c, int from, int to )
{
	return _duc_cmd_succ(c, "artistmerge %d %d", from, to );
}

int duc_cmd_artistadd( dudlc *c, const char *name )
{
	return _duc_cmd_int(c, "artistadd %s", name );
}

int duc_cmd_artistdel( dudlc *c, int id )
{
	return _duc_cmd_succ(c, "artistdel %d", id );
}



