/*
 * Copyright (c) 2008 Rainer Clasen
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

#ifndef _DUDLC_ALBUM_H
#define _DUDLC_ALBUM_H

#include "session.h"
#include "iterate.h"
#include "artist.h"

typedef struct {
	int id;
	char *album;
	duc_artist *artist;
	int year;
} duc_album;

duc_album *_duc_album_parse( char *line, char **end );
void duc_album_free( duc_album *a );
DEFINE_DUC_ITER_PARSE(album);

duc_album *duc_cmd_albumget( dudlc *c, int id );

duc_it_album *duc_cmd_albumlist( dudlc *c );
duc_it_album *duc_cmd_albumsearch( dudlc *c, const char *substr );
duc_it_album *duc_cmd_albumsartist( dudlc *c, int artistid );
duc_it_album *duc_cmd_albumstag( dudlc *c, int tid );

int duc_cmd_albumsetname( dudlc *c, int id, const char *name );
int duc_cmd_albumsetartist( dudlc *c, int id, int artistid );
int duc_cmd_albumsetyear( dudlc *c, int id, int year );

#endif
