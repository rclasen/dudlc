#ifndef _MSERVCLIENT_QUEUE_H
#define _MSERVCLIENT_QUEUE_H

#include <mservclient/client.h>
#include <mservclient/track.h>

typedef struct {
	int id;
	int uid;
	int queued;
	msc_track *_track;
} msc_queue;

void msc_queue_free( msc_queue *q );

#define msc_it_queue	_msc_iter
#define msc_it_queue_cur(x)	(msc_queue*)_msc_it_cur((_msc_iter*)x)
#define msc_it_queue_next(x)	(msc_queue*)_msc_it_next((_msc_iter*)x)
#define msc_it_queue_done(x)	_msc_it_done((_msc_iter*)x)


msc_it_queue *msc_cmd_queue( mservclient *c );
msc_queue *msc_cmd_queueget( mservclient *c, int qid );

int msc_cmd_queuealbum( mservclient *c, int id );
int msc_cmd_queueadd( mservclient *c, int tid );
int msc_cmd_queuedel( mservclient *c, int qid );
int msc_cmd_queueclear( mservclient *c );

void _msc_bcast_queue( mservclient *c, const char *line );

#endif
