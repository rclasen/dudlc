#ifndef _DUDLC_QUEUE_H
#define _DUDLC_QUEUE_H

#include "session.h"
#include "user.h"
#include "track.h"

typedef struct {
	int id;
	int queued;
	duc_user *user;
	duc_track *track;
} duc_queue;

void duc_queue_free( duc_queue *q );

#define duc_it_queue	_duc_iter
#define duc_it_queue_cur(x)	(duc_queue*)_duc_it_cur((_duc_iter*)x)
#define duc_it_queue_next(x)	(duc_queue*)_duc_it_next((_duc_iter*)x)
#define duc_it_queue_done(x)	_duc_it_done((_duc_iter*)x)


duc_it_queue *duc_cmd_queuelist( dudlc *c );
duc_queue *duc_cmd_queueget( dudlc *c, int qid );

int duc_cmd_queuealbum( dudlc *c, int id );
int duc_cmd_queueadd( dudlc *c, int tid );
int duc_cmd_queuedel( dudlc *c, int qid );
int duc_cmd_queueclear( dudlc *c );

void _duc_bcast_queue( dudlc *c, const char *line );

#endif
