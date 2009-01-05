/*
 * Copyright (c) 2008 Rainer Clasen
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

#ifndef _DUDLC_TRACK_H
#define _DUDLC_TRACK_H

#include "session.h"
#include "iterate.h"
#include "artist.h"
#include "album.h"

typedef struct {
	int id;
	duc_album *album;
	int albumnr;
	char *title;
	duc_artist *artist;
	int duration;
} duc_track;

duc_track *_duc_track_parse( char *line, char **end );
void duc_track_free( duc_track *t );
DEFINE_DUC_ITER_PARSE(track);

int duc_cmd_tracks( dudlc *c );
int duc_cmd_track2id( dudlc *c, int albumid, int nr );
duc_track *duc_cmd_trackget( dudlc *c, int id );

duc_it_track *duc_cmd_tracksearch( dudlc *c, const char *substr );
duc_it_track *duc_cmd_tracksearchf( dudlc *c, const char *filter );
duc_it_track *duc_cmd_tracksalbum( dudlc *c, int id );
duc_it_track *duc_cmd_tracksartist( dudlc *c, int id );

int duc_cmd_tracksetname( dudlc *c, int id, const char *name );
int duc_cmd_tracksetartist( dudlc *c, int id, int artistid );

#endif
