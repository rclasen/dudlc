#ifndef _DUDLC_PLAYER_H
#define _DUDLC_PLAYER_H

#include "session.h"

#include "track.h"


typedef enum {
	pl_stop,
	pl_play,
	pl_pause,
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

void _duc_bcast_player( dudlc *c, char *line );

#endif
