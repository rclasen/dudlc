#ifndef _MSERVCLIENT_PLAYER_H
#define _MSERVCLIENT_PLAYER_H

#include <mservclient/session.h>

#include <mservclient/track.h>


typedef enum {
	pl_stop,
	pl_play,
	pl_pause,
} msc_playstatus;

msc_playstatus msc_cmd_status( mservclient *c );
msc_track *msc_cmd_curtrack( mservclient *c );

int msc_cmd_play( mservclient *c );
int msc_cmd_stop( mservclient *c );
int msc_cmd_next( mservclient *c );
int msc_cmd_prev( mservclient *c );
int msc_cmd_pause( mservclient *c );

int msc_cmd_gap( mservclient *c );
int msc_cmd_gapset( mservclient *c, unsigned int gap );

int msc_cmd_random( mservclient *c );
int msc_cmd_randomset( mservclient *c, int r );

void _msc_bcast_player( mservclient *c, const char *line );

#endif
