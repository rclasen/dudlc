#include <stdlib.h>
#include <string.h>

#include <mservclient/proto.h>
#include <mservclient/event.h>
#include <mservclient/album.h>

msc_album *_msc_album_parse( const char *line, char **end )
{
	msc_album *n;
	const char *s;
	char *e;

	/* why isn't strtol's endptr defined as (const char*) ?
	 * now we have to cast the const hackishly away */
	(const char*)e = s = line;

	if( NULL == (n = malloc(sizeof(msc_album)))){
		goto clean1;
	}

	n->id = strtol( s, &e, 10 );
	if( s == e )
		goto clean2;

	s = e+1;
	if( NULL == ( n->album = _msc_fielddup( s, &e )))
		goto clean2;

	s = e+1;
	n->artistid = strtol( s, &e, 10 );
	if( s == e )
		goto clean3;

	if( end ) *end = e;
	return n;

clean3:
	free(n->album);
clean2:
	free(n);
clean1:
	if( end ) (const char *)*end = line;
	return NULL;
}

void msc_album_free( msc_album *a )
{
	free(a->album);
	free(a);
}

msc_album *msc_cmd_albumget( mservclient *c, int id )
{
	return _msc_cmd_conv( c, (_msc_converter)_msc_album_parse,
			"albumget %d", id );
}

msc_it_album *msc_cmd_albumlist( mservclient *c )
{
	return _msc_iterate( c, (_msc_converter)_msc_album_parse, 
			"albumlist" );
}

msc_it_album *msc_cmd_albumsearch( mservclient *c, const char *str )
{
	return _msc_iterate( c, (_msc_converter)_msc_album_parse, 
			"albumsearch %s", str );
}

int msc_cmd_albumsetname( mservclient *c, int id, const char *name )
{
	return _msc_cmd_succ(c, "albumsetname %d %s", id, name );
}

int msc_cmd_albumsetartist( mservclient *c, int id, int artistid )
{
	return _msc_cmd_succ(c, "albumsetartist %d %d", id, artistid );
}



