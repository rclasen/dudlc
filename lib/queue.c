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
#include "dudlc/queue.h"

static duc_queue *_duc_queue_parse( char *line, char **end )
{
	duc_queue *n;
	char *s;
	char *e;

	e = s = line;

	if( NULL == (n = malloc(sizeof(duc_queue)))){
		goto clean1;
	}

	n->id = strtol( s, &e, 10 );
	if( s == e )
		goto clean2;

	s = e+1;
	n->queued = strtol( s, &e, 10 );
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

void duc_queue_free( duc_queue *q )
{
	if( ! q )
		return;
	duc_user_free(q->user);
	duc_track_free(q->track);
	free(q);
}

duc_it_queue *duc_cmd_queuelist( dudlc *c )
{
	return _duc_iterate( c, (_duc_converter)_duc_queue_parse, 
			"queuelist" );
}

duc_queue *duc_cmd_queueget( dudlc *c, int qid )
{
	return _duc_cmd_conv( c, (_duc_converter)_duc_queue_parse,
			"queueget %d", qid );
}

int duc_cmd_queueadd( dudlc *c, int id )
{
	return _duc_cmd_int( c, "queueadd %d", id );
}

int duc_cmd_queuealbum( dudlc *c, int id )
{
	duc_it_track *it;
	duc_track *t;
	int *ids = NULL;
	int total = 0;
	int found = 0;
	int i;

	if( NULL == (it = duc_cmd_tracksalbum(c,id)))
		return -1;

	for( t = duc_it_track_cur(it); t; t = duc_it_track_next(it)){
		if( found >= total ){
			int *tmp;

			total += 10;
			tmp = realloc( ids, sizeof(int*) * total );
			if( NULL == tmp ){
				free(t);
				break;
			}
			ids = tmp;

		}
		ids[found++] = t->id;
		free(t);
	}
	duc_it_track_done(it);

	for( i = 0; i < found; ++i ){
		if( 0 > duc_cmd_queueadd(c, ids[i]) ){
			free(ids);
			return -1;
		}
	}

	free(ids);
	return 0;
}

int duc_cmd_queuedel( dudlc *c, int id )
{
	return _duc_cmd_succ( c, "queuedel %d", id );
}

int duc_cmd_queueclear( dudlc *c )
{
	return _duc_cmd_succ( c, "queueclear" );
}

int duc_cmd_queuesum( dudlc *c )
{
	return _duc_cmd_int( c, "queuesum" );
}

void _duc_bcast_queue( dudlc *c, char *line )
{
	duc_queue *q;

	switch(line[2]){
		case '0': /* fetched from queue */
			if( NULL == (q = _duc_queue_parse(line+4,NULL)))
				break;
			_MSC_EVENT(c,queuefetch,c,q);
			duc_queue_free(q);
			break;

		case '1': /* added to queue */
			if( NULL == (q = _duc_queue_parse(line+4,NULL)))
				break;
			_MSC_EVENT(c,queueadd,c,q);
			duc_queue_free(q);
			break;

		case '2': /* deleted from queue */
			if( NULL == (q = _duc_queue_parse(line+4,NULL)))
				break;
			_MSC_EVENT(c,queuedel,c,q);
			duc_queue_free(q);
			break;

		case '3': /* queue cleared */
			_MSC_EVENT(c,queueclear,c);
			break;


		default:
			_MSC_EVENT(c,bcast,c,line);
	}
}


