#ifndef _DUDLC_ARTIST_H
#define _DUDLC_ARTIST_H

#include "session.h"
#include "iterate.h"

typedef struct {
	int id;
	char *artist;
} duc_artist;

#define duc_it_artist	_duc_iter
#define duc_it_artist_cur(x)	(duc_artist*)_duc_it_cur((_duc_iter*)x)
#define duc_it_artist_next(x)	(duc_artist*)_duc_it_next((_duc_iter*)x)
#define duc_it_artist_done(x)	_duc_it_done((_duc_iter*)x)

duc_artist *_duc_artist_parse( const char *line, char **end );
void duc_artist_free( duc_artist *a );

duc_artist *duc_cmd_artistget( dudlc *c, int id );

duc_it_artist *duc_cmd_artistlist( dudlc *c );
duc_it_artist *duc_cmd_artistsearch( dudlc *c, const char *substr );

int duc_cmd_artistsetname( dudlc *c, int id, const char *name );
int duc_cmd_artistadd( dudlc *c, const char *name );
int duc_cmd_artistdel( dudlc *c, int id );

#endif
