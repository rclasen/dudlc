#include <stdlib.h>
#include <string.h>

#include "dudlc/proto.h"
#include "dudlc/event.h"
#include "dudlc/history.h"

static duc_history *_duc_history_parse( const char *line, char **end )
{
	duc_history *n;
	const char *s;
	char *e;

	/* why isn't strtol's endptr defined as (const char*) ?
	 * now we have to cast the const hackishly away */
	(const char*)e = s = line;

	if( NULL == (n = malloc(sizeof(duc_history)))){
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
	if( NULL == ( n->_track = _duc_track_parse( s, &e )))
		goto clean2;


	if( end ) *end = e;
	return n;

clean2:
	free(n);
clean1:
	if( end ) (const char *)*end = line;
	return NULL;
}

void duc_history_free( duc_history *q )
{
	if( ! q )
		return;
	duc_track_free(q->_track);
	free(q);
}

duc_it_history *duc_cmd_history( dudlc *c, int num )
{
	return _duc_iterate( c, (_duc_converter)_duc_history_parse, 
			"history %d", num );
}

duc_it_history *duc_cmd_historytrack( dudlc *c, int tid, int num )
{
	return _duc_iterate( c, (_duc_converter)_duc_history_parse,
			"historytrack %d %d", tid, num );
}



