/*
 * Copyright (c) 2008 Rainer Clasen
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

#ifndef _DUDLC_PLAYER_H
#define _DUDLC_PLAYER_H

#include "session.h"

#include "track.h"


typedef enum {
	pl_stop,
	pl_play,
	pl_pause,
	pl_offline,
} duc_playstatus;

duc_playstatus duc_cmd_status( dudlc *c );
duc_track *duc_cmd_curtrack( dudlc *c );

int duc_cmd_play( dudlc *c );
int duc_cmd_stop( dudlc *c );
int duc_cmd_next( dudlc *c );
int duc_cmd_prev( dudlc *c );
int duc_cmd_pause( dudlc *c );

int duc_cmd_gap( dudlc *c );
int duc_cmd_gapset( dudlc *c, unsigned int gap );

int duc_cmd_random( dudlc *c );
int duc_cmd_randomset( dudlc *c, int r );

int duc_cmd_elapsed( dudlc *c );
int duc_cmd_jump( dudlc *c, int r );

int duc_cmd_cut( dudlc *c );
int duc_cmd_cutset( dudlc *c, int r );
int duc_cmd_replaygain( dudlc *c );
int duc_cmd_replaygainset( dudlc *c, int r );
int duc_cmd_rgpreamp( dudlc *c );
int duc_cmd_rgpreampset( dudlc *c, double r );

void _duc_bcast_player( dudlc *c, char *line );

#endif
