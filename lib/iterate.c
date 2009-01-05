/*
 * Copyright (c) 2008 Rainer Clasen
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */


#include <stdlib.h>
#include <stdarg.h>

#include "dudlc/proto.h"
#include "dudlc/iterate.h"

_duc_iter *_duc_it_new( dudlc *p, 
	_duc_converter conv, 
	_duc_free_func freefunc,
	const char *cmd, ... )
{
	_duc_iter *it;
	va_list ap;

	va_start(ap, cmd );
	it = _duc_it_new( p, conv, freefunc, cmd, ap );
	va_end(ap);

	return it;
}

_duc_iter *_duc_it_newv( dudlc *p, 
	_duc_converter conv, 
	_duc_free_func freefunc,
	const char *cmd, va_list ap )
{
	_duc_iter *it;
	struct _duc_it_datalist *last = NULL;
	char *line;

	if( ! p || ! freefunc )
		return NULL;

	/* submit command */
	if( _duc_vsend( p, cmd, ap ) )
		return NULL;

	if( _duc_rnext(p) )
		return NULL;

	if( _duc_rcode(p)[0] == '5' )
		goto clean1;

	/* success, parse all responses */
	if( NULL == (it = malloc(sizeof(_duc_iter))))
		goto clean1;

	it->list = NULL;
	it->freefunc = freefunc;

	do {
		struct _duc_it_datalist *next;
		char *end;

		if( NULL == (line = _duc_rline(p)))
			goto clean2;

		if( NULL == (next = malloc(sizeof(struct _duc_it_datalist)) ) )
			goto clean2;

		if( it->list == NULL ){
			last = it->list = next;
		} else {
			last->next = next;
			last = next;
		}	

		last->next = NULL;
		last->data = (*conv)( line, &end );
		if( *end != 0 )
			goto clean2;

	} while( 0 <= _duc_rnext(p) );
		

	return it;
clean2:
	_duc_it_done(it);

clean1:
	_duc_rlast(p);
	return NULL;
}

void *_duc_it_cur( _duc_iter *i )
{
	if( ! i || ! i->list )
		return NULL;

	return i->list->data;
}

void *_duc_it_next( _duc_iter *i )
{
	struct _duc_it_datalist *cur;

	if( ! i || ! i->list )
		return NULL;

	cur = i->list;
	i->list = cur->next;

	/* 
	 * gives away ownership of data, so don't free it:
	 * (*i->freefunc)(cur->data);
	 */

	return i->list->data;
}

void _duc_it_done( _duc_iter *i )
{
	struct _duc_it_datalist *cur;

	if( ! i )
		return;
	
	cur = i->list; 
	while( cur ){
		if( i->freefunc)
			(*i->freefunc)(cur->data);

		i->list = cur->next;
		free(cur);

		cur=i->list;
	}

	free(i);
}



