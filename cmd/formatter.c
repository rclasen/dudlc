#include <stdio.h>
#include <time.h>

#include "dudlccmd.h"

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

const char *dfmt_clienthead( char *buf, unsigned int len )
{
	snprintf( buf, len, "%4.4s %4.4s %-15s", "id", "uid", "address" );
	return buf;
}

// TODO: lookup user name
const char *dfmt_client( char *buf, unsigned int len, duc_client *c )
{
	snprintf( buf, len, "%4d %4d %-15s", c->id, c->uid, c->addr );
	return buf;
}

/************************************************************
 * track
 */

const char *dfmt_trackid( int albumid, int nr )
{
	static char buf[40];

	snprintf( buf, 40, "%d/%d", albumid, nr );
	return buf;
}

const char *dfmt_trackhead( char *buf, unsigned int len )
{
	snprintf( buf, len, "%7.7s %-18.18s %-5.5s %-16.16s %-29.29s",
			"alb/nr", "album", "dur", "artist", "title" );
	return buf;
}

#define BF	"\x1B[1m"
#define NF	"\x1B[0m"
const char *dfmt_rtrack( char *buf, unsigned int len, 
		duc_track *t, duc_artist *ar, duc_album *al )
{
	char tim[10];

	minutes(tim, 10, t->duration);
	snprintf( buf, len, 
			"%7s "
			BF "%-18.18s " NF
			"%-5.5s "
			BF "%-16.16s " NF
			"%-29.29s",
			dfmt_trackid(t->albumid, t->albumnr), 
			al->album, tim, ar->artist, t->title );

	return buf;
}

// TODO: lookup artist and album name
// TODO: include duration
const char *dfmt_track( char *buf, unsigned int len, duc_track *t )
{
	char tim[10];

	minutes(tim, 10, t->duration);
	snprintf( buf, len, "%7s %-18s %5s %-16d %-29.29s",
			dfmt_trackid(t->albumid, t->albumnr), "",
			tim,
			t->artistid, t->title );

	return buf;
}

/************************************************************
 * queue
 */

const char *dfmt_queuehead( char *buf, unsigned int len )
{
	unsigned int l;

	l = snprintf( buf, len, "%4.4s %4.4s %-8.8s ", "id", "uid", "queued" );
	if( l > len )
		return NULL;

	dfmt_trackhead( buf+l, len-l );
	return buf;
}

const char *dfmt_queue( char *buf, unsigned int len, duc_queue *q )
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

	dfmt_track( buf+l, len-l, q->_track );
	return buf;
}

/************************************************************
 * history
 */

const char *dfmt_historyhead( char *buf, unsigned int len )
{
	unsigned int l;

	l = snprintf( buf, len, "%4.4s %-19.19s ", "uid", "played" );
	if( l > len )
		return NULL;

	dfmt_trackhead( buf+l, len-l );
	return buf;
}

const char *dfmt_history( char *buf, unsigned int len, duc_history *q )
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

	dfmt_track( buf+l, len-l, q->_track );
	return buf;
}

/************************************************************
 * tag
 */

const char *dfmt_taghead( char *buf, unsigned int len )
{
	unsigned int l;

	l = snprintf( buf, len, "%4.4s %-20s %-20s", "id", "name", "desc" );
	if( l > len )
		return NULL;

	return buf;
}

const char *dfmt_tag( char *buf, unsigned int len, duc_tag *q )
{
	unsigned int l;

	l = snprintf( buf, len, "%4d %-20s %-20s", q->id, q->name, q->desc );
	if( l > len )
		return NULL;

	return buf;
}

/************************************************************
 * user
 */

const char *dfmt_userhead( char *buf, unsigned int len )
{
	unsigned int l;

	l = snprintf( buf, len, "%4.4s %-20s %s", "id", "name", "right" );
	if( l > len )
		return NULL;

	return buf;
}

const char *dfmt_user( char *buf, unsigned int len, duc_user *q )
{
	unsigned int l;

	l = snprintf( buf, len, "%4d %-20s %2d", q->id, q->name, q->right );
	if( l > len )
		return NULL;

	return buf;
}


/************************************************************
 * album
 */

const char *dfmt_albumhead( char *buf, unsigned int len )
{
	unsigned int l;

	l = snprintf( buf, len, "%4.4s %-25s %-20s", "id", "album", "artist" );
	if( l > len )
		return NULL;

	return buf;
}

// TODO: lookup artist
const char *dfmt_album( char *buf, unsigned int len, duc_album *q )
{
	unsigned int l;

	l = snprintf( buf, len, "%4d %-25s %-20d", q->id, q->album, q->artistid);
	if( l > len )
		return NULL;

	return buf;
}


/************************************************************
 * artist
 */

const char *dfmt_artisthead( char *buf, unsigned int len )
{
	unsigned int l;

	l = snprintf( buf, len, "%4.4s %-20s", "id", "artist" );
	if( l > len )
		return NULL;

	return buf;
}

const char *dfmt_artist( char *buf, unsigned int len, duc_artist *q )
{
	unsigned int l;

	l = snprintf( buf, len, "%4d %-20s", q->id, q->artist);
	if( l > len )
		return NULL;

	return buf;
}


