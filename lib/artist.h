#ifndef _MSERVCLIENT_ARTIST_H
#define _MSERVCLIENT_ARTIST_H

#include <mservclient/session.h>
#include <mservclient/iterate.h>

typedef struct {
	int id;
	char *artist;
} msc_artist;

#define msc_it_artist	_msc_iter
#define msc_it_artist_cur(x)	(msc_artist*)_msc_it_cur((_msc_iter*)x)
#define msc_it_artist_next(x)	(msc_artist*)_msc_it_next((_msc_iter*)x)
#define msc_it_artist_done(x)	_msc_it_done((_msc_iter*)x)

msc_artist *_msc_artist_parse( const char *line, char **end );
void msc_artist_free( msc_artist *a );

msc_artist *msc_cmd_artistget( mservclient *c, int id );

msc_it_artist *msc_cmd_artistlist( mservclient *c );
msc_it_artist *msc_cmd_artistsearch( mservclient *c, const char *substr );

int msc_cmd_artistsetname( mservclient *c, int id, const char *name );
int msc_cmd_artistadd( mservclient *c, const char *name );
int msc_cmd_artistdel( mservclient *c, int id );

#endif
