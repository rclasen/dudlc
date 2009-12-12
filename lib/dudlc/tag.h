/*
 * Copyright (c) 2008 Rainer Clasen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

#ifndef _DUDLC_TAG_H
#define _DUDLC_TAG_H

#include "session.h"

typedef struct {
	int id;
	char *name;
	char *desc;
} duc_tag;

duc_tag *_duc_tag_parse( char *line, char **end );
void duc_tag_free( duc_tag *t );
DEFINE_DUC_ITER_PARSE(tag);

duc_tag *duc_cmd_tagget( dudlc *c, int id );
int duc_cmd_tag2id( dudlc *c, const char *name );

duc_it_tag *duc_cmd_taglist( dudlc *c );
duc_it_tag *duc_cmd_tagsartist( dudlc *c, int aid );

int duc_cmd_tagadd( dudlc *c, const char *name );
int duc_cmd_tagsetname( dudlc *c, int id, const char *name );
int duc_cmd_tagsetdesc( dudlc *c, int id, const char *desc );
int duc_cmd_tagdel( dudlc *c, int id );

duc_it_tag *duc_cmd_tracktaglist( dudlc *c, int tid );
int duc_cmd_tracktagged( dudlc *c, int tid, int id );

int duc_cmd_tracktagadd( dudlc *c, int tid, int id );
int duc_cmd_tracktagdel( dudlc *c, int tid, int id );

void _duc_bcast_tag( dudlc *c, char *line );


#endif
