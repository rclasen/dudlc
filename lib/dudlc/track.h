#ifndef _DUDLC_TRACK_H
#define _DUDLC_TRACK_H

#include "session.h"
#include "iterate.h"

typedef struct {
	int id;
	int albumid;
	int albumnr;
	char *title;
	int artistid;
	int duration;
} duc_track;

#define duc_it_track	_duc_iter
#define duc_it_track_cur(x)	(duc_track*)_duc_it_cur((_duc_iter*)x)
#define duc_it_track_next(x)	(duc_track*)_duc_it_next((_duc_iter*)x)
#define duc_it_track_done(x)	_duc_it_done((_duc_iter*)x)

duc_track *_duc_track_parse( const char *line, char **end );
void duc_track_free( duc_track *t );

int duc_cmd_tracks( dudlc *c );
int duc_cmd_track2id( dudlc *c, int albumid, int nr );
duc_track *duc_cmd_trackget( dudlc *c, int id );

duc_it_track *duc_cmd_tracksearch( dudlc *c, const char *substr );
duc_it_track *duc_cmd_tracksalbum( dudlc *c, int id );
duc_it_track *duc_cmd_tracksartist( dudlc *c, int id );

// TODO: alter 


#endif
