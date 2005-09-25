#include <stdlib.h>
#include <string.h>

#include "dudlc/proto.h"
#include "dudlc/event.h"
#include "dudlc/album.h"

duc_album *_duc_album_parse( char *line, char **end )
{
	duc_album *n;
	char *s;
	char *e;

	e = s = line;

	if( NULL == (n = malloc(sizeof(duc_album)))){
		goto clean1;
	}

	n->id = strtol( s, &e, 10 );
	if( s == e )
		goto clean2;

	s = e+1;
	if( NULL == ( n->album = _duc_fielddup( s, &e )))
		goto clean2;

	s = e+1;
	if( NULL == (n->artist = _duc_artist_parse(s, &e )))
		goto clean3;

	if( end ) *end = e;
	return n;

clean3:
	free(n->album);
clean2:
	free(n);
clean1:
	if( end ) *end = line;
	return NULL;
}

void duc_album_free( duc_album *a )
{
	duc_artist_free(a->artist);
	free(a->album);
	free(a);
}

duc_album *duc_cmd_albumget( dudlc *c, int id )
{
	return _duc_cmd_conv( c, (_duc_converter)_duc_album_parse,
			"albumget %d", id );
}

duc_it_album *duc_cmd_albumlist( dudlc *c )
{
	return _duc_iterate( c, (_duc_converter)_duc_album_parse, 
			"albumlist" );
}

duc_it_album *duc_cmd_albumsearch( dudlc *c, const char *str )
{
	return _duc_iterate( c, (_duc_converter)_duc_album_parse, 
			"albumsearch %s", str );
}

duc_it_album *duc_cmd_albumsartist( dudlc *c, int artistid )
{
	return _duc_iterate( c, (_duc_converter)_duc_album_parse, 
			"albumsartist %d", artistid );
}

// TODO: duc_cmd_albumsearchf

int duc_cmd_albumsetname( dudlc *c, int id, const char *name )
{
	return _duc_cmd_succ(c, "albumsetname %d %s", id, name );
}

int duc_cmd_albumsetartist( dudlc *c, int id, int artistid )
{
	return _duc_cmd_succ(c, "albumsetartist %d %d", id, artistid );
}



