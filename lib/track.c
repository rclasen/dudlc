
#include <stdlib.h>
#include <string.h>

#include <mservclient/proto.h>
#include <mservclient/track.h>

msc_track *_msc_track_parse( const char *line, char **end )
{
	msc_track *t;
	const char *s;
	char *e;

	/* why isn't strtol's endptr defined as (const char*) ?
	 * now we have to cast the const hackishly away */
	(const char*)e = s = line;

	if( NULL == (t = malloc(sizeof(msc_track)))){
		goto clean1;
	}

	t->id = strtol( s, &e, 10 );
	if( s == e )
		goto clean2;

	s = e+1;
	t->albumid = strtol( s, &e, 10 );
	if( s == e )
		goto clean2;

	s = e+1;
	t->albumnr = strtol( s, &e, 10 );
	if( s == e )
		goto clean3;

	s = e+1;
	if( NULL == (t->title = _msc_fielddup( s, &e )))
		goto clean2;

	s = e+1;
	t->artistid = strtol( s, &e, 10 );
	if( s == e )
		goto clean3;

	s = e+1;
	t->duration = strtol( s, &e, 10 );
	if( s == e )
		goto clean3;


	if( end ) *end = e;
	return t;

clean3:
	free(t->title);
clean2:
	free(t);
clean1:
	if( end ) (const char *)*end = line;
	return NULL;
}


void msc_track_free( msc_track *t )
{
	if( ! t )
		return;

	free(t->title);
	free(t);
}

int msc_cmd_tracks( mservclient *c )
{
	return _msc_cmd_int( c, "tracks" );
}

int msc_cmd_trackid( mservclient *c, int albumid, int nr )
{
	return _msc_cmd_int( c, "trackid %d %d", albumid, nr );
}

msc_track *msc_cmd_trackget( mservclient *c, int id )
{
	return _msc_cmd_conv( c, (_msc_converter)_msc_track_parse,
			"trackget %d", id );
}

msc_it_track *msc_cmd_tracksearch( mservclient *c, const char *substr )
{
	return _msc_iterate( c, (_msc_converter)_msc_track_parse, 
			"tracksearch %s", substr );
}

msc_it_track *msc_cmd_tracksalbum( mservclient *c, int id )
{
	return _msc_iterate( c, (_msc_converter)_msc_track_parse, 
			"tracksalbum %d", id );
}

msc_it_track *msc_cmd_tracksartist( mservclient *c, int id )
{
	return _msc_iterate( c, (_msc_converter)_msc_track_parse, 
			"tracksartist %d", id );
}




