
#include <stdlib.h>
#include <string.h>

#include "dudlc/proto.h"
#include "dudlc/track.h"

duc_track *_duc_track_parse( char *line, char **end )
{
	duc_track *t;
	char *s;
	char *e;

	e = s = line;

	if( NULL == (t = malloc(sizeof(duc_track)))){
		goto clean1;
	}

	t->id = strtol( s, &e, 10 );
	if( s == e )
		goto clean2;

	s = e+1;
	t->albumnr = strtol( s, &e, 10 );
	if( s == e )
		goto clean2;

	s = e+1;
	if( NULL == (t->title = _duc_fielddup( s, &e )))
		goto clean2;

	s = e+1;
	t->duration = strtol( s, &e, 10 );
	if( s == e )
		goto clean3;

	s = e+1;
	if( NULL == (t->artist = _duc_artist_parse( s, &e )))
		goto clean3;

	s = e+1;
	if( NULL == (t->album = _duc_album_parse( s, &e )))
		goto clean4;


	if( end ) *end = e;
	return t;

clean4:
	duc_artist_free(t->artist);
clean3:
	free(t->title);
clean2:
	free(t);
clean1:
	if( end ) *end = line;
	return NULL;
}


void duc_track_free( duc_track *t )
{
	if( ! t )
		return;

	duc_album_free(t->album);
	duc_artist_free(t->artist);
	free(t->title);
	free(t);
}

int duc_cmd_tracks( dudlc *c )
{
	return _duc_cmd_int( c, "trackcount" );
}

int duc_cmd_track2id( dudlc *c, int albumid, int nr )
{
	return _duc_cmd_int( c, "track2id %d %d", albumid, nr );
}

duc_track *duc_cmd_trackget( dudlc *c, int id )
{
	return _duc_cmd_conv( c, (_duc_converter)_duc_track_parse,
			"trackget %d", id );
}

duc_it_track *duc_cmd_tracksearch( dudlc *c, const char *substr )
{
	return _duc_iterate( c, (_duc_converter)_duc_track_parse, 
			"tracksearch %s", substr );
}

duc_it_track *duc_cmd_tracksearchf( dudlc *c, const char *filter )
{
	return _duc_iterate( c, (_duc_converter)_duc_track_parse, 
			"tracksearchf %s", filter );
}

duc_it_track *duc_cmd_tracksalbum( dudlc *c, int id )
{
	return _duc_iterate( c, (_duc_converter)_duc_track_parse, 
			"tracksalbum %d", id );
}

duc_it_track *duc_cmd_tracksartist( dudlc *c, int id )
{
	return _duc_iterate( c, (_duc_converter)_duc_track_parse, 
			"tracksartist %d", id );
}




