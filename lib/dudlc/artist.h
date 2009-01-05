/*
 * Copyright (c) 2008 Rainer Clasen
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

#ifndef _DUDLC_ARTIST_H
#define _DUDLC_ARTIST_H

#include "session.h"
#include "iterate.h"

typedef struct {
	int id;
	char *artist;
} duc_artist;

duc_artist *_duc_artist_parse( char *line, char **end );
void duc_artist_free( duc_artist *a );
DEFINE_DUC_ITER_PARSE(artist);

duc_artist *duc_cmd_artistget( dudlc *c, int id );

duc_it_artist *duc_cmd_artistlist( dudlc *c );
duc_it_artist *duc_cmd_artiststag( dudlc *c, int tid );
duc_it_artist *duc_cmd_artistsearch( dudlc *c, const char *substr );

int duc_cmd_artistsetname( dudlc *c, int id, const char *name );
int duc_cmd_artistmerge( dudlc *c, int from, int to);
int duc_cmd_artistadd( dudlc *c, const char *name );
int duc_cmd_artistdel( dudlc *c, int id );

#endif
