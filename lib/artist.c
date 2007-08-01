#include <stdlib.h>
#include <string.h>

#include "dudlc/proto.h"
#include "dudlc/event.h"
#include "dudlc/artist.h"

duc_artist *_duc_artist_parse( char *line, char **end )
{
	duc_artist *n;
	char *s;
	char *e;

	e = s = line;

	if( NULL == (n = malloc(sizeof(duc_artist)))){
		goto clean1;
	}

	n->id = strtol( s, &e, 10 );
	if( s == e )
		goto clean2;

	s = e+1;
	if( NULL == ( n->artist = _duc_fielddup( s, &e )))
		goto clean2;

	if( end ) *end = e;
	return n;

clean2:
	free(n);
clean1:
	if( end ) *end = line;
	return NULL;
}

void duc_artist_free( duc_artist *a )
{
	free(a->artist);
	free(a);
}

duc_artist *duc_cmd_artistget( dudlc *c, int id )
{
	return _duc_cmd_conv( c, (_duc_converter)_duc_artist_parse,
			"artistget %d", id );
}

duc_it_artist *duc_cmd_artistlist( dudlc *c )
{
	return _duc_iterate( c, (_duc_converter)_duc_artist_parse, 
			"artistlist" );
}

duc_it_artist *duc_cmd_artiststag( dudlc *c, int tid )
{
	return _duc_iterate( c, (_duc_converter)_duc_artist_parse, 
			"artiststag %d", tid );
}

duc_it_artist *duc_cmd_artistsearch( dudlc *c, const char *str )
{
	return _duc_iterate( c, (_duc_converter)_duc_artist_parse, 
			"artistsearch %s", str );
}

int duc_cmd_artistsetname( dudlc *c, int id, const char *name )
{
	return _duc_cmd_succ(c, "artistsetname %d %s", id, name );
}

int duc_cmd_artistmerge( dudlc *c, int from, int to )
{
	return _duc_cmd_succ(c, "artistmerge %d %d", from, to );
}

int duc_cmd_artistadd( dudlc *c, const char *name )
{
	return _duc_cmd_int(c, "artistadd %s", name );
}

int duc_cmd_artistdel( dudlc *c, int id )
{
	return _duc_cmd_succ(c, "artistdel %d", id );
}



