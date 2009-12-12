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
#include "dudlc/tag.h"

duc_it_tag *_duc_it_tag_new( dudlc *p, const char *cmd, ... )
{
	_duc_iter *it;
	va_list ap;

	va_start(ap,cmd);
	it = _duc_it_newv(p,
		(_duc_converter)_duc_tag_parse,
		(_duc_free_func)duc_tag_free,
		cmd, ap );
	va_end(ap);

	return it;
}

duc_tag *_duc_tag_parse( char *line, char **end )
{
	duc_tag *n;
	char *s;
	char *e;

	e = s = line;

	if( NULL == (n = malloc(sizeof(duc_tag))))
		goto clean1;
	memset( n, 0, sizeof(duc_tag) );

	n->id = strtol( s, &e, 10 );
	if( s == e )
		goto clean2;

	s = e+1;
	if( NULL == ( n->name = _duc_fielddup(s, &e)))
		goto clean2;

	s = e+1;
	if( NULL == ( n->desc = _duc_fielddup(s, &e)))
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

void duc_tag_free( duc_tag *n )
{
	if( ! n )
		return;
	free(n->name);
	free(n->desc);
	free(n);
}

duc_tag *duc_cmd_tagget( dudlc *c, int id )
{
	return _duc_cmd_conv(c, (_duc_converter)_duc_tag_parse,
			"tagget %d", id );
}

int duc_cmd_tag2id( dudlc *c, const char *name )
{
	return _duc_cmd_int(c, "tag2id %s", name );
}


duc_it_tag *duc_cmd_taglist( dudlc *c )
{
	return _duc_it_tag_new(c, "taglist" );
}

duc_it_tag *duc_cmd_tagsartist( dudlc *c, int aid )
{
	return _duc_it_tag_new(c, "tagsartist %d", aid );
}

int duc_cmd_tagadd( dudlc *c, const char *name )
{
	return _duc_cmd_int(c, "tagadd %s", name );
}

int duc_cmd_tagsetname( dudlc *c, int id, const char *name )
{
	return _duc_cmd_succ( c, "tagsetname %d %s", id, name );
}

int duc_cmd_tagsetdesc( dudlc *c, int id, const char *desc )
{
	return _duc_cmd_succ( c, "tagsetdesc %d %s", id, desc );
}

int duc_cmd_tagdel( dudlc *c, int id )
{
	return _duc_cmd_succ( c, "tagdel %d", id );
}


duc_it_tag *duc_cmd_tracktaglist( dudlc *c, int tid )
{
	return _duc_it_tag_new(c, "tracktaglist %d", tid );
}

int duc_cmd_tracktagged( dudlc *c, int tid, int id )
{
	return _duc_cmd_int(c, "tracktagged %d %d", tid, id );
}

int duc_cmd_tracktagadd( dudlc *c, int tid, int id )
{
	return _duc_cmd_succ( c, "tracktagadd %d %d", tid, id );
}

int duc_cmd_tracktagdel( dudlc *c, int tid, int id )
{
	return _duc_cmd_succ( c, "tracktagdel %d %d", tid, id );
}

void _duc_bcast_tag( dudlc *c, char *line )
{
	duc_tag *t;

	switch(line[2]){
		case '0': /* tag changed */
			if( NULL == (t = _duc_tag_parse(line+4,NULL)))
				break;
			_MSC_EVENT(c,tagchange,c,t);
			duc_tag_free(t);
			break;

		case '1': /* tag changed */
			if( NULL == (t = _duc_tag_parse(line+4,NULL)))
				break;
			_MSC_EVENT(c,tagdel,c,t);
			duc_tag_free(t);
			break;


		default:
			_MSC_EVENT(c,bcast,c,line);
	}
}




