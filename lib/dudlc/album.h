#ifndef _MSERVCLIENT_ALBUM_H
#define _MSERVCLIENT_ALBUM_H

#include <mservclient/session.h>
#include <mservclient/iterate.h>

typedef struct {
	int id;
	char *album;
	int artistid;
} msc_album;

#define msc_it_album	_msc_iter
#define msc_it_album_cur(x)	(msc_album*)_msc_it_cur((_msc_iter*)x)
#define msc_it_album_next(x)	(msc_album*)_msc_it_next((_msc_iter*)x)
#define msc_it_album_done(x)	_msc_it_done((_msc_iter*)x)

msc_album *_msc_album_parse( const char *line, char **end );
void msc_album_free( msc_album *a );

msc_album *msc_cmd_albumget( mservclient *c, int id );

msc_it_album *msc_cmd_albumlist( mservclient *c );
msc_it_album *msc_cmd_albumsearch( mservclient *c, const char *substr );

int msc_cmd_albumsetname( mservclient *c, int id, const char *name );
int msc_cmd_albumsetartist( mservclient *c, int id, int artistid );

#endif
