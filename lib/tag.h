#ifndef _MSERVCLIENT_TAG_H
#define _MSERVCLIENT_TAG_H

#include <mservclient/client.h>

typedef struct {
	int id;
	char *name;
	char *desc;
} msc_tag;

#define msc_it_tag	_msc_iter
#define msc_it_tag_cur(x)	(msc_tag*)_msc_it_cur((_msc_iter*)x)
#define msc_it_tag_next(x)	(msc_tag*)_msc_it_next((_msc_iter*)x)
#define msc_it_tag_done(x)	_msc_it_done((_msc_iter*)x)

void msc_tag_free( msc_tag *t );

msc_tag *msc_cmd_tagget( mservclient *c, int id );
msc_tag *msc_cmd_tagname( mservclient *c, const char *name );

msc_it_tag *msc_cmd_taglist( mservclient *c );

int msc_cmd_tagadd( mservclient *c, const char *name );
int msc_cmd_tagsetname( mservclient *c, int id, const char *name );
int msc_cmd_tagsetdesc( mservclient *c, int id, const char *desc );
int msc_cmd_tagdel( mservclient *c, int id );

msc_it_tag *msc_cmd_tracktags( mservclient *c, int tid );
int msc_cmd_tracktagged( mservclient *c, int tid, int id );

int msc_cmd_tracktagset( mservclient *c, int tid, int id );
int msc_cmd_tracktagdel( mservclient *c, int tid, int id );

void _msc_bcast_tag( mservclient *c, const char *line );


#endif
