
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
	(const char*)e = s = _msc_skipspace(line);

	if( NULL == (t = malloc(sizeof(msc_track)))){
		goto clean1;
	}

	t->id = strtol( s, &e, 10 );
	if( s == e )
		goto clean2;

	s = _msc_skipspace(e);
	t->albumid = strtol( s, &e, 10 );
	if( s == e )
		goto clean2;

	s = _msc_skipspace(e);
	t->albumnr = strtol( s, &e, 10 );
	if( s == e )
		goto clean3;

	s = _msc_skipspace(e);
	if( NULL == (t->title = _msc_fielddup( s, &e )))
		goto clean2;

	s = _msc_skipspace(e);
	t->artistid = strtol( s, &e, 10 );
	if( s == e )
		goto clean3;

	s = _msc_skipspace(e);
	t->duration = strtol( s, &e, 10 );
	if( s == e )
		goto clean3;

	return t;

clean3:
	free(t->title);
clean2:
	free(t);
clean1:
	if( end ) *end = e;
	return NULL;
}


void msc_track_free( msc_track *t )
{
	if( ! t )
		return;

	if( --t->_refs )
		return;

	free(t->title);
	free(t);
}

int msc_cmd_tracks( mservclient *c )
{
	if( _msc_cmd( c, "tracks" ))
		return -1;

	if( *_msc_rcode(c) != '2' )
		return -1;

	return atoi(_msc_rline(c));
}

msc_track *msc_cmd_trackget( mservclient *c, int id )
{
	if( _msc_cmd( c, "trackget %d", id ))
		return NULL;

	if( *_msc_rcode(c) != '2' )
		return NULL;

	return _msc_track_parse(_msc_rline(c),NULL);
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




