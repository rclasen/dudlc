#ifndef _MSERVCLIENT_RANDOM_H
#define _MSERVCLIENT_RANDOM_H

#include "session.h"
#include "track.h"

char *msc_cmd_filter( mservclient *c );
int msc_cmd_filterset( mservclient *c, const char *filter );
int msc_cmd_filterstat( mservclient *c );

msc_it_track *msc_cmd_randomtop( mservclient *c, int num );

void _msc_bcast_random( mservclient *c, const char *line );

#endif
