#include <stdlib.h>
#include <string.h>

#include <mservclient/proto.h>
#include <mservclient/event.h>
#include <mservclient/queue.h>

static msc_queue *_msc_queue_parse( const char *line, char **end )
{
	msc_queue *n;
	const char *s;
	char *e;

	/* why isn't strtol's endptr defined as (const char*) ?
	 * now we have to cast the const hackishly away */
	(const char*)e = s = line;

	if( NULL == (n = malloc(sizeof(msc_track)))){
		goto clean1;
	}

	n->id = strtol( s, &e, 10 );
	if( s == e )
		goto clean2;

	s = e+1;
	n->uid = strtol( s, &e, 10 );
	if( s == e )
		goto clean2;

	s = e+1;
	n->queued = strtol( s, &e, 10 );
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

void msc_queue_free( msc_queue *q )
{
	if( ! q )
		return;
	msc_track_free(q->_track);
	free(q);
}

msc_it_queue *msc_cmd_queue( mservclient *c )
{
	return _msc_iterate( c, (_msc_converter)_msc_queue_parse, "queue" );
}

msc_queue *msc_cmd_queueget( mservclient *c, int qid )
{
	if( _msc_cmd( c, "queueget %d", qid ))
		return NULL;
	if( *_msc_rcode(c) != '2' )
		return NULL-1;
	return _msc_queue_parse(_msc_rline(c), NULL);
}

int msc_cmd_queueadd( mservclient *c, int id )
{
	if( _msc_cmd( c, "queueadd %d", id ))
		return -1;
	if( *_msc_rcode(c) != '2' )
		return -1;
	return atoi(_msc_rline(c));
}

int msc_cmd_queuealbum( mservclient *c, int id )
{
	msc_it_track *it;
	msc_track *t;
	int *ids = NULL;
	int total = 0;
	int found = 0;
	int i;

	if( NULL == (it = msc_cmd_tracksalbum(c,id)))
		return -1;

	for( t = msc_it_track_cur(it); t; t = msc_it_track_next(it)){
		if( found >= total ){
			int *tmp;

			total += 10;
			tmp = realloc( ids, sizeof(int*) * total );
			if( NULL == tmp ){
				free(t);
				break;
			}
			ids = tmp;

		}
		ids[found++] = t->id;
		free(t);
	}
	msc_it_track_done(it);

	for( i = 0; i < found; ++i ){
		if( 0 > msc_cmd_queueadd(c, ids[i]) ){
			free(ids);
			return -1;
		}
	}

	free(ids);
	return 0;
}

int msc_cmd_queuedel( mservclient *c, int id )
{
	if( _msc_cmd( c, "queuedel %d", id ))
		return -1;
	if( *_msc_rcode(c) != '2' )
		return -1;
	return 0;
}

int msc_cmd_queueclear( mservclient *c )
{
	if( _msc_cmd( c, "queueclear" ))
		return -1;
	if( *_msc_rcode(c) != '2' )
		return -1;
	return 0;
}

void _msc_bcast_queue( mservclient *c, const char *line )
{
	msc_queue *q;

	switch(line[2]){
		case '0': /* fetched from queue */
			if( NULL == (q = _msc_queue_parse(line+4,NULL)))
				break;
			_MSC_EVENT(c,queuefetch,c,q);
			msc_queue_free(q);
			break;

		case '1': /* added to queue */
			if( NULL == (q = _msc_queue_parse(line+4,NULL)))
				break;
			_MSC_EVENT(c,queueadd,c,q);
			msc_queue_free(q);
			break;

		case '2': /* deleted from queue */
			if( NULL == (q = _msc_queue_parse(line+4,NULL)))
				break;
			_MSC_EVENT(c,queuedel,c,q);
			msc_queue_free(q);
			break;

		case '3': /* queue cleared */
			_MSC_EVENT(c,queueclear,c);
			break;


		default:
			_MSC_EVENT(c,bcast,c,line);
	}
}


