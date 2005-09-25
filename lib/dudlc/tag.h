#ifndef _DUDLC_TAG_H
#define _DUDLC_TAG_H

#include "session.h"

typedef struct {
	int id;
	char *name;
	char *desc;
} duc_tag;

#define duc_it_tag	_duc_iter
#define duc_it_tag_cur(x)	(duc_tag*)_duc_it_cur((_duc_iter*)x)
#define duc_it_tag_next(x)	(duc_tag*)_duc_it_next((_duc_iter*)x)
#define duc_it_tag_done(x)	_duc_it_done((_duc_iter*)x)

void duc_tag_free( duc_tag *t );

duc_tag *duc_cmd_tagget( dudlc *c, int id );
int duc_cmd_tag2id( dudlc *c, const char *name );

duc_it_tag *duc_cmd_taglist( dudlc *c );

int duc_cmd_tagadd( dudlc *c, const char *name );
int duc_cmd_tagsetname( dudlc *c, int id, const char *name );
int duc_cmd_tagsetdesc( dudlc *c, int id, const char *desc );
int duc_cmd_tagdel( dudlc *c, int id );

duc_it_tag *duc_cmd_tracktaglist( dudlc *c, int tid );
int duc_cmd_tracktagged( dudlc *c, int tid, int id );

int duc_cmd_tracktagset( dudlc *c, int tid, int id );
int duc_cmd_tracktagdel( dudlc *c, int tid, int id );

void _duc_bcast_tag( dudlc *c, char *line );


#endif
