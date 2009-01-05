/*
 * Copyright (c) 2008 Rainer Clasen
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

#ifndef _DUDLC_PROTO_H
#define _DUDLC_PROTO_H

#include <stdarg.h>

#include "session.h"

/************************************************************
 * internal protocol parsing functions
 ************************************************************/

typedef void *(*_duc_converter)( const char *line, char **end );
typedef void *(*_duc_free_func)( void * );

int _duc_vsend( dudlc *p, const char *fmt, va_list ap );
int _duc_send( dudlc *p, const char *fmt, ... );

int _duc_rlast( dudlc *p );
int _duc_rnext( dudlc *p );
int _duc_rend( dudlc *p );
char *_duc_rcode( dudlc *p );
char *_duc_rline( dudlc *p );

#define duc_rmsg _duc_rline

/* convenience pack for single-line responses: */
int _duc_cmd( dudlc *p, const char *fmt, ... );
int _duc_cmd_succ( dudlc *c, const char *fmt, ... );
int _duc_cmd_int( dudlc *c, const char *fmt, ... );
double _duc_cmd_double( dudlc *c, const char *fmt, ... );
char *_duc_cmd_string( dudlc *c, const char *fmt, ... );
void *_duc_cmd_conv( dudlc *c, _duc_converter conv, 
		const char *fmt, ... );


char *_duc_fielddup( char *s, char **end );
char *_duc_skipspace( char *s );

#endif

