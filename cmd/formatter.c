/*
 * Copyright (c) 2008 Rainer Clasen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */


#include <stdio.h>
#include <time.h>

#include "dudlccmd.h"

// TODO: give user chance to influence column width
// TODO: adjust column width to window size

char *dfmt_bf = NULL;
char *dfmt_nf = NULL;

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

	/* TODO: use plain snprintf + modulo divison % instead of strftime */
	t = gmtime( &time );
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

char *dfmt_clienthead( char *buf, unsigned int len )
{
	snprintf( buf, len, "%4.4s %-10s %-15s", "id", "user", "address" );
	return buf;
}

char *dfmt_client( char *buf, unsigned int len, duc_client *c )
{
	snprintf( buf, len, "%4d %-10s %-15s", c->id, c->user->name, c->addr );
	return buf;
}

/************************************************************
 * track
 */

char *dfmt_trackid( int albumid, int nr )
{
	static char buf[40];

	snprintf( buf, 40, "%d/%d", albumid, nr );
	return buf;
}

char *dfmt_trackhead( char *buf, unsigned int len )
{
	snprintf( buf, len, "%7.7s %-18.18s %-5.5s %-16.16s %-29s",
			"alb/nr", "album", "dur", "artist", "title" );
	return buf;
}

#define BF	(dfmt_bf ? dfmt_bf : "")
#define NF	(dfmt_nf ? dfmt_nf : "")
char *dfmt_track( char *buf, unsigned int len, duc_track *t )
{
	char tim[10];

	minutes(tim, 10, t->duration);
	snprintf( buf, len,
			"%7s "
			"%s%-18.18s%s "
			"%-5.5s "
			"%s%-16.16s%s "
			"%-29s",
			dfmt_trackid(t->album->id, t->albumnr),
			BF, t->album->album, NF,
			tim,
			BF, t->artist->artist, NF,
			t->title );

	return buf;
}


/************************************************************
 * queue
 */

char *dfmt_queuehead( char *buf, unsigned int len )
{
	unsigned int l;

	l = snprintf( buf, len, "%4.4s %-10s %-8.8s ", "id", "user", "queued" );
	if( l > len )
		return NULL;

	dfmt_trackhead( buf+l, len-l );
	return buf;
}

char *dfmt_queue( char *buf, unsigned int len, duc_queue *q )
{
	unsigned int l;

	l = snprintf( buf, len, "%4d %-10s ", q->id, q->user->name );
	if( l > len )
		return NULL;

	l += isotime( buf+l, len -l, q->queued );
	if( l +2 > len )
		return NULL;

	buf[l++] = ' ';
	buf[l] = 0;

	dfmt_track( buf+l, len-l, q->track );
	return buf;
}

/************************************************************
 * history
 */

char *dfmt_historyhead( char *buf, unsigned int len )
{
	unsigned int l;

	l = snprintf( buf, len, "%-19.19s %-10s ", "played", "user" );
	if( l > len )
		return NULL;

	dfmt_trackhead( buf+l, len-l );
	return buf;
}

char *dfmt_history( char *buf, unsigned int len, duc_history *q )
{
	unsigned int l;

	l = isotimestamp( buf, len, q->played );
	if( l +2 > len )
		return NULL;

	l += snprintf( buf+l, len-l, " %-10s ", q->user->name );
	if( l > len )
		return NULL;

	dfmt_track( buf+l, len-l, q->track );
	return buf;
}

/************************************************************
 * tag
 */

char *dfmt_taghead( char *buf, unsigned int len )
{
	snprintf( buf, len, "%4.4s %-20s %-20s", "id", "name", "desc" );
	return buf;
}

char *dfmt_tag( char *buf, unsigned int len, duc_tag *q )
{
	snprintf( buf, len, "%4d %-20s %-20s", q->id, q->name, q->desc );
	return buf;
}

/************************************************************
 * user
 */

char *dfmt_userhead( char *buf, unsigned int len )
{
	snprintf( buf, len, "%4.4s %-20s %s", "id", "name", "right" );
	return buf;
}

char *dfmt_user( char *buf, unsigned int len, duc_user *q )
{
	snprintf( buf, len, "%4d %-20s %2d", q->id, q->name, q->right );
	return buf;
}


/************************************************************
 * album
 */

char *dfmt_albumhead( char *buf, unsigned int len )
{
	snprintf( buf, len, "%4.4s %4s %-25s %-20s", "id", "year", "artist", "album" );
	return buf;
}

char *dfmt_album( char *buf, unsigned int len, duc_album *q )
{
	snprintf( buf, len, "%4d %4d %-25s %-20s",
			q->id, q->year, q->artist->artist, q->album);
	return buf;
}


/************************************************************
 * artist
 */

char *dfmt_artisthead( char *buf, unsigned int len )
{
	snprintf( buf, len, "%4.4s %-20s", "id", "artist" );
	return buf;
}

char *dfmt_artist( char *buf, unsigned int len, duc_artist *q )
{
	snprintf( buf, len, "%4d %-20s", q->id, q->artist);
	return buf;
}

/************************************************************
 * sfilter
 */

char *dfmt_sfilterhead( char *buf, unsigned int len )
{
	snprintf( buf, len, "%4.4s %-20s %s", "id", "name", "sfilter" );
	return buf;
}

char *dfmt_sfilter( char *buf, unsigned int len, duc_sfilter *q )
{
	snprintf( buf, len, "%4d %-20s %s", q->id, q->name, q->filter );
	return buf;
}


