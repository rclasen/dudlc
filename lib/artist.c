#include <stdlib.h>
#include <string.h>

#include <mservclient/proto.h>
#include <mservclient/event.h>
#include <mservclient/artist.h>

msc_artist *_msc_artist_parse( const char *line, char **end )
{
	msc_artist *n;
	const char *s;
	char *e;

	/* why isn't strtol's endptr defined as (const char*) ?
	 * now we have to cast the const hackishly away */
	(const char*)e = s = line;

	if( NULL == (n = malloc(sizeof(msc_artist)))){
		goto clean1;
	}

	n->id = strtol( s, &e, 10 );
	if( s == e )
		goto clean2;

	s = e+1;
	if( NULL == ( n->artist = _msc_fielddup( s, &e )))
		goto clean2;

	if( end ) *end = e;
	return n;

clean2:
	free(n);
clean1:
	if( end ) (const char *)*end = line;
	return NULL;
}

void msc_artist_free( msc_artist *a )
{
	free(a->artist);
	free(a);
}

msc_artist *msc_cmd_artistget( mservclient *c, int id )
{
	return _msc_cmd_conv( c, (_msc_converter)_msc_artist_parse,
			"artistget %d", id );
}

msc_it_artist *msc_cmd_artistlist( mservclient *c )
{
	return _msc_iterate( c, (_msc_converter)_msc_artist_parse, 
			"artistlist" );
}

msc_it_artist *msc_cmd_artistsearch( mservclient *c, const char *str )
{
	return _msc_iterate( c, (_msc_converter)_msc_artist_parse, 
			"artistsearch %s", str );
}

int msc_cmd_artistsetname( mservclient *c, int id, const char *name )
{
	return _msc_cmd_succ(c, "artistsetname %d %s", id, name );
}

int msc_cmd_artistadd( mservclient *c, const char *name )
{
	return _msc_cmd_int(c, "artistadd %s", name );
}

int msc_cmd_artistdel( mservclient *c, int id )
{
	return _msc_cmd_succ(c, "artistdel %d", id );
}



