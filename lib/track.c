
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include <mservclient/proto.h>
#include <mservclient/track.h>

static const char *_msc_skipspace( const char *s )
{
	while( *s && isspace(*s))
		s++;

	return s;
}

static char *_msc_fielddup( const char *s, char **end )
{
	char *dup;
	char *p;
	int escape = 0;

	/* alloc a string large enough for escaped string - the result
	 * will be smaller */
	if( NULL == (dup = malloc(1 + strcspn(s,"\t"))))
		return NULL;

	p = dup;
	for(; *s && *s != '\t'; ++s ){
		if( escape ){
			switch( *s ){
				case 't':
					*p++ = '\t';
					break;
				case '\\':
				default:
					*p++ = *s;
					break;
			}
			escape = 0;
			continue;
		} 

		if( *s == '\\' ){
			escape++;
			continue;
		}

		*p++ = *s;
	}
	*p++= 0;

	if( end ) (const char *)*end = s;

	return dup;
}

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

	s = _msc_skipspace(e);
	t->lastplay = strtol( s, &e, 10 );
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

msc_track *msc_cmd_track_get( mservclient *c, int id )
{
	if( _msc_cmd( c, "trackget %d", id ))
		return NULL;

	if( *_msc_rcode(c) != '2' )
		return NULL;

	return _msc_track_parse(_msc_rline(c),NULL);
}

msc_it_track *msc_cmd_tracks_search( mservclient *c, const char *substr )
{
	return _msc_iterate( c, (_msc_converter)_msc_track_parse, 
			"tracksearch %s", substr );
}

msc_it_track *msc_cmd_tracks_albumid( mservclient *c, int id )
{
	return _msc_iterate( c, (_msc_converter)_msc_track_parse, 
			"tracksalbum %d", id );
}

msc_it_track *msc_cmd_tracks_artistid( mservclient *c, int id )
{
	return _msc_iterate( c, (_msc_converter)_msc_track_parse, 
			"tracksartist %d", id );
}




