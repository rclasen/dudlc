#include <stdio.h>
#include <time.h>

#include "tty.h"
#include "formatter.h"

static size_t isotime( char *s, size_t max, time_t time )
{
	struct tm *t;

	t = localtime( &time );
	return strftime( s, max, "%H:%M:%S", t );
	/*                        HH:MM:SS */
	/*                        8 */
}


static size_t minutes( char *s, size_t max, time_t time )
{
	struct tm *t;

	t = localtime( &time );
	return strftime( s, max, "%M:%S", t );
	/*                        MM:SS */
	/*                        5 */
}


static size_t isotimestamp( char *s, size_t max, time_t time )
{
	struct tm *t;

	t = localtime( &time );
	return strftime( s, max, "%Y-%m-%d %H:%M:%S", t );
	/*                      YYYY-mm-dd HH:MM:SS */
	/*                      10         8 */
}

#if 0
static size_t isodate( char *s, size_t max, time_t time )
{
	struct tm *t;

	t = localtime( &time );
	return strftime( buf, max, "%Y-%m-%d", t );
}
#endif

/************************************************************
 * client
 */

const char *mkclienthead( char *buf, unsigned int len )
{
	snprintf( buf, len, "%4.4s %4.4s %-15s", "id", "uid", "address" );
	return buf;
}

// TODO: lookup user name
const char *mkclient( char *buf, unsigned int len, msc_client *c )
{
	snprintf( buf, len, "%4d %4d %-15s", c->id, c->uid, c->addr );
	return buf;
}

void dump_clients( msc_it_client *it )
{
	char buf[BUFLENCLIENT];
	msc_client *c;

	tty_msg( "%s\n\n", mkclienthead(buf, BUFLENCLIENT));
	for( c = msc_it_client_cur(it); c; c = msc_it_client_next(it)){
		tty_msg( "%s\n", mkclient(buf, BUFLENCLIENT, c ));
		msc_client_free(c);
	}
}

/************************************************************
 * track
 */

const char *mktrackid( int albumid, int nr )
{
	static char buf[40];

	snprintf( buf, 40, "%d/%d", albumid, nr );
	return buf;
}

const char *mktrackhead( char *buf, unsigned int len )
{
	snprintf( buf, len, "%7.7s %-18.18s %-5.5s %-16.16s %-29.29s",
			"alb/nr", "album", "dur", "artist", "title" );
	return buf;
}

const char *mkrtrack( char *buf, unsigned int len, 
		msc_track *t, msc_artist *ar, msc_album *al )
{
	char tim[10];

	minutes(tim, 10, t->duration);
	snprintf( buf, len, "%7s %-18.18s %-5.5s %-16.16s %-29.29s",
			mktrackid(t->albumid, t->albumnr), 
			al->album, tim, ar->artist, t->title );

	return buf;
}

// TODO: lookup artist and album name
// TODO: include duration
const char *mktrack( char *buf, unsigned int len, msc_track *t )
{
	char tim[10];

	minutes(tim, 10, t->duration);
	snprintf( buf, len, "%7s %-18s %5s %-16d %-29.29s",
			mktrackid(t->albumid, t->albumnr), "",
			tim,
			t->artistid, t->title );

	return buf;
}

void dump_tracks( msc_it_track *it )
{
	msc_track *t;
	char buf[BUFLENTRACK];

	tty_msg( "%s\n\n", mktrackhead(buf, BUFLENTRACK));
	for( t = msc_it_track_cur(it); t; t = msc_it_track_next(it)){
		tty_msg( "%s\n", mktrack(buf,BUFLENTRACK,t));
		msc_track_free(t);
	}
}

/************************************************************
 * queue
 */

const char *mkqueuehead( char *buf, unsigned int len )
{
	unsigned int l;

	l = snprintf( buf, len, "%4.4s %4.4s %-8.8s ", "id", "uid", "queued" );
	if( l > len )
		return NULL;

	mktrackhead( buf+l, len-l );
	return buf;
}

const char *mkqueue( char *buf, unsigned int len, msc_queue *q )
{
	unsigned int l;

	l = snprintf( buf, len, "%4d %4d ", q->id, q->uid );
	if( l > len )
		return NULL;

	l += isotime( buf+l, len -l, q->queued );
	if( l +2 > len )
		return NULL;

	buf[l++] = ' ';
	buf[l] = 0;

	mktrack( buf+l, len-l, q->_track );
	return buf;
}

void dump_queue( msc_it_queue *it )
{
	msc_queue *t;
	char buf[BUFLENQUEUE];

	tty_msg( "%s\n\n", mkqueuehead(buf, BUFLENQUEUE));
	for( t = msc_it_queue_cur(it); t; t = msc_it_queue_next(it)){
		tty_msg( "%s\n", mkqueue(buf,BUFLENQUEUE,t));
		msc_queue_free(t);
	}
}


/************************************************************
 * history
 */

const char *mkhistoryhead( char *buf, unsigned int len )
{
	unsigned int l;

	l = snprintf( buf, len, "%4.4s %-19.19s ", "uid", "played" );
	if( l > len )
		return NULL;

	mktrackhead( buf+l, len-l );
	return buf;
}

const char *mkhistory( char *buf, unsigned int len, msc_history *q )
{
	unsigned int l;

	l = snprintf( buf, len, "%4d ", q->uid );
	if( l > len )
		return NULL;

	l += isotimestamp( buf+l, len -l, q->played );
	if( l +2 > len )
		return NULL;

	buf[l++] = ' ';
	buf[l] = 0;

	mktrack( buf+l, len-l, q->_track );
	return buf;
}

void dump_history( msc_it_history *it )
{
	msc_history *t;
	char buf[BUFLENQUEUE];

	tty_msg( "%s\n\n", mkhistoryhead(buf, BUFLENQUEUE));
	for( t = msc_it_history_cur(it); t; t = msc_it_history_next(it)){
		tty_msg( "%s\n", mkhistory(buf,BUFLENQUEUE,t));
		msc_history_free(t);
	}
}


/************************************************************
 * tag
 */

const char *mktaghead( char *buf, unsigned int len )
{
	unsigned int l;

	l = snprintf( buf, len, "%4.4s %-20s %-20s", "id", "name", "desc" );
	if( l > len )
		return NULL;

	return buf;
}

const char *mktag( char *buf, unsigned int len, msc_tag *q )
{
	unsigned int l;

	l = snprintf( buf, len, "%4d %-20s %-20s", q->id, q->name, q->desc );
	if( l > len )
		return NULL;

	return buf;
}

void dump_tags( msc_it_tag *it )
{
	msc_tag *t;
	char buf[BUFLENTAG];

	tty_msg( "%s\n\n", mktaghead(buf, BUFLENTAG));
	for( t = msc_it_tag_cur(it); t; t = msc_it_tag_next(it)){
		tty_msg( "%s\n", mktag(buf,BUFLENTAG,t));
		msc_tag_free(t);
	}
}

/************************************************************
 * user
 */

const char *mkuserhead( char *buf, unsigned int len )
{
	unsigned int l;

	l = snprintf( buf, len, "%4.4s %-20s %s", "id", "name", "right" );
	if( l > len )
		return NULL;

	return buf;
}

const char *mkuser( char *buf, unsigned int len, msc_user *q )
{
	unsigned int l;

	l = snprintf( buf, len, "%4d %-20s %2d", q->id, q->name, q->right );
	if( l > len )
		return NULL;

	return buf;
}

void dump_users( msc_it_user *it )
{
	msc_user *t;
	char buf[BUFLENUSER];

	tty_msg( "%s\n\n", mkuserhead(buf, BUFLENUSER));
	for( t = msc_it_user_cur(it); t; t = msc_it_user_next(it)){
		tty_msg( "%s\n", mkuser(buf,BUFLENUSER,t));
		msc_user_free(t);
	}
}

