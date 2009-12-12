/*
 * Copyright (c) 2008 Rainer Clasen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

#ifndef _DUDLC_HISTORY_H
#define _DUDLC_HISTORY_H

#include "session.h"
#include "user.h"
#include "track.h"

typedef struct {
	int played;
	duc_track *track;
	duc_user *user;
} duc_history;

duc_history *_duc_history_parse( char *line, char **end );
void duc_history_free( duc_history *h );
DEFINE_DUC_ITER_PARSE(history);

duc_it_history *duc_cmd_history(dudlc *c, int num );
duc_it_history *duc_cmd_historytrack( dudlc *c, int trackid, int num );

#endif
