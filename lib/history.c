#include <stdlib.h>
#include <string.h>

#include <mservclient/proto.h>
#include <mservclient/event.h>
#include <mservclient/history.h>

static msc_history *_msc_history_parse( const char *line, char **end )
{
	msc_history *n;
	const char *s;
	char *e;

	/* why isn't strtol's endptr defined as (const char*) ?
	 * now we have to cast the const hackishly away */
	(const char*)e = s = line;

	if( NULL == (n = malloc(sizeof(msc_track)))){
		goto clean1;
	}

	n->uid = strtol( s, &e, 10 );
	if( s == e )
		goto clean2;

	s = e+1;
	n->played = strtol( s, &e, 10 );
	if( s == e )
		goto clean2;

	s = e+1;
	if( NULL == ( n->_track = _msc_track_parse( s, &e )))
		goto clean2;


	if( end ) *end = e;
	return n;

clean2:
	free(n);
clean1:
	if( end ) (const char *)*end = line;
	return NULL;
}

void msc_history_free( msc_history *q )
{
	if( ! q )
		return;
	msc_track_free(q->_track);
	free(q);
}

msc_it_history *msc_cmd_history( mservclient *c, int num )
{
	return _msc_iterate( c, (_msc_converter)_msc_history_parse, 
			"history %d", num );
}

msc_it_history *msc_cmd_historytrack( mservclient *c, int tid, int num )
{
	return _msc_iterate( c, (_msc_converter)_msc_history_parse,
			"historytrack %d %d", tid, num );
}



