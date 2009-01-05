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

typedef struct {
	dudlc *con;
	_duc_converter conv;
} _duc_iter;

_duc_iter *_duc_iterate( dudlc *p, _duc_converter conv, 
		const char *cmd, ... );
void *_duc_it_cur( _duc_iter *i );
void *_duc_it_next( _duc_iter *i );
void _duc_it_done( _duc_iter *i );

#define DEFINE_DUC_ITER(name) \
typedef _duc_iter	duc_it_##name; \
static inline duc_##name *duc_it_##name##_cur( duc_it_##name *it) \
	{ return (duc_##name*)_duc_it_cur( (_duc_iter*)it ); } \
static inline duc_##name *duc_it_##name##_next( duc_it_##name *it) \
	{ return (duc_##name*)_duc_it_next( (_duc_iter*)it ); } \
static inline void duc_it_##name##_done( duc_it_##name * it ) \
	{ _duc_it_done( (_duc_iter*)it );  }

#define DEFINE_DUC_ITER_PARSE(name) \
	DEFINE_DUC_ITER(name)


#endif
