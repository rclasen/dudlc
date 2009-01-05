/*
 * Copyright (c) 2008 Rainer Clasen
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

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

duc_queue *_duc_queue_parse( char *line, char **end );
void duc_queue_free( duc_queue *q );
DEFINE_DUC_ITER_PARSE(queue);


duc_it_queue *duc_cmd_queuelist( dudlc *c );
duc_queue *duc_cmd_queueget( dudlc *c, int qid );

int duc_cmd_queuealbum( dudlc *c, int id );
int duc_cmd_queueadd( dudlc *c, int tid );
int duc_cmd_queuedel( dudlc *c, int qid );
int duc_cmd_queueclear( dudlc *c );
int duc_cmd_queuesum( dudlc *c );

void _duc_bcast_queue( dudlc *c, char *line );

#endif
