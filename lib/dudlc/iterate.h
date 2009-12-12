/*
 * Copyright (c) 2008 Rainer Clasen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

#ifndef _DUDLC_ITERATE_H
#define _DUDLC_ITERATE_H

#include "session.h"
#include "proto.h"

/*
 * _duc_iter: standard iterator for dudl responses
 *
 * reads response from server while iterating. Response is parsed by
 * _duc_converter function which returns a newly allocated response
 * struct. This means you cannot submit new commands while you're
 * iterating!
 *
 * iterator takes care of finishing to read the server response in
 * duc_it_done.
 *
 */
struct _duc_it_datalist {
	void *data;
	struct _duc_it_datalist *next;
};

typedef struct {
	struct _duc_it_datalist *list;
	_duc_free_func freefunc;
} _duc_iter;


/*
 * _duc_it_new: submit comand to server and return new iterator to read
 * the results
 */
_duc_iter *_duc_it_new( dudlc *p,
	_duc_converter conv,
	_duc_free_func free,
	const char *cmd, ... );
_duc_iter *_duc_it_newv( dudlc *p,
	_duc_converter conv,
	_duc_free_func free,
	const char *cmd, va_list ap );
/* parse last received line and return response structure */
void *_duc_it_cur( _duc_iter *i );
/* read next line, parse it and return response struct */
void *_duc_it_next( _duc_iter *i );
/* free iterator (after reading finishing to read the server response) */
void _duc_it_done( _duc_iter *i );


/*
 * helper to define typesafe iterator for custom result structures
 *
 * <name> becomes
 * row typedef:		duc_<name>
 * iterator typedef:	duc_it_<name>
 * proto methods:	_duc_it_<name>_new
 * inline methods:	duc_it_<name>_(cur|next|done)
 */
/* TODO: introduce duc_it_##name##_new( con, cmd, ... ) */
#define DEFINE_DUC_ITER(name) \
typedef _duc_iter	duc_it_##name; \
duc_it_##name *_duc_it_##name##_new( dudlc *p, const char *cmd, ... ); \
static inline duc_##name *duc_it_##name##_cur( duc_it_##name *it) \
{ \
	return (duc_##name*)_duc_it_cur( (_duc_iter*)it );\
} \
static inline duc_##name *duc_it_##name##_next( duc_it_##name *it) \
{ \
	return (duc_##name*)_duc_it_next( (_duc_iter*)it );\
} \
static inline void duc_it_##name##_done( duc_it_##name * it ) \
{ \
	_duc_it_done( (_duc_iter*)it );\
}

/*
 * for now: same as above. Will include definition of convert- + free- functions
 */
#define DEFINE_DUC_ITER_PARSE(name) \
	DEFINE_DUC_ITER(name)


#endif
