#ifndef _MSERVCLIENT_HISTORY_H
#define _MSERVCLIENT_HISTORY_H

#include <mservclient/session.h>
#include <mservclient/track.h>

typedef struct {
	int uid;
	int played;
	msc_track *_track;
} msc_history;

void msc_history_free( msc_history *h );

#define msc_it_history	_msc_iter
#define msc_it_history_cur(x)	(msc_history*)_msc_it_cur((_msc_iter*)x)
#define msc_it_history_next(x)	(msc_history*)_msc_it_next((_msc_iter*)x)
#define msc_it_history_done(x)	_msc_it_done((_msc_iter*)x)

msc_it_history *msc_cmd_history(mservclient *c, int num );
msc_it_history *msc_cmd_historytrack( mservclient *c, int trackid, int num );

#endif
