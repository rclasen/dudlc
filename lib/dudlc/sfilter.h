/*
 * Copyright (c) 2008 Rainer Clasen
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

#ifndef _DUDLC_SFILTER_H
#define _DUDLC_SFILTER_H

#include "session.h"

typedef struct {
	int id;
	char *name;
	char *filter;
} duc_sfilter;

duc_sfilter *_duc_sfilter_parse( char *line, char **end );
void duc_sfilter_free( duc_sfilter *t );
DEFINE_DUC_ITER_PARSE(sfilter);

duc_sfilter *duc_cmd_sfilterget( dudlc *c, int id );
int duc_cmd_sfilter2id( dudlc *c, const char *name );

duc_it_sfilter *duc_cmd_sfilterlist( dudlc *c );

int duc_cmd_sfilteradd( dudlc *c, const char *name );
int duc_cmd_sfiltersetname( dudlc *c, int id, const char *name );
int duc_cmd_sfiltersetfilter( dudlc *c, int id, const char *filter );
int duc_cmd_sfilterdel( dudlc *c, int id );

#endif
