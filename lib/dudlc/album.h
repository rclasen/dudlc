#ifndef _DUDLC_ALBUM_H
#define _DUDLC_ALBUM_H

#include "session.h"
#include "iterate.h"

typedef struct {
	int id;
	char *album;
	int artistid;
} duc_album;

#define duc_it_album	_duc_iter
#define duc_it_album_cur(x)	(duc_album*)_duc_it_cur((_duc_iter*)x)
#define duc_it_album_next(x)	(duc_album*)_duc_it_next((_duc_iter*)x)
#define duc_it_album_done(x)	_duc_it_done((_duc_iter*)x)

duc_album *_duc_album_parse( const char *line, char **end );
void duc_album_free( duc_album *a );

duc_album *duc_cmd_albumget( dudlc *c, int id );

duc_it_album *duc_cmd_albumlist( dudlc *c );
duc_it_album *duc_cmd_albumsearch( dudlc *c, const char *substr );

int duc_cmd_albumsetname( dudlc *c, int id, const char *name );
int duc_cmd_albumsetartist( dudlc *c, int id, int artistid );

#endif