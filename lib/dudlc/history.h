#ifndef _DUDLC_HISTORY_H
#define _DUDLC_HISTORY_H

#include "session.h"
#include "track.h"

typedef struct {
	int uid;
	int played;
	duc_track *_track;
} duc_history;

void duc_history_free( duc_history *h );

#define duc_it_history	_duc_iter
#define duc_it_history_cur(x)	(duc_history*)_duc_it_cur((_duc_iter*)x)
#define duc_it_history_next(x)	(duc_history*)_duc_it_next((_duc_iter*)x)
#define duc_it_history_done(x)	_duc_it_done((_duc_iter*)x)

duc_it_history *duc_cmd_history(dudlc *c, int num );
duc_it_history *duc_cmd_historytrack( dudlc *c, int trackid, int num );

#endif
