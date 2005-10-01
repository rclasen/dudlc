#ifndef _DUDLCCMD_H
#define _DUDLCCMD_H

#include <dudlc.h>

const char *dfmt_clienthead( char *buf, unsigned int len );
const char *dfmt_client( char *buf, unsigned int len, duc_client *c );

const char *dfmt_trackid( int albumid, int nr );

const char *dfmt_trackhead( char *buf, unsigned int len );
const char *dfmt_track( char *buf, unsigned int len, duc_track *t );

const char *dfmt_queuehead( char *buf, unsigned int len );
const char *dfmt_queue( char *buf, unsigned int len, duc_queue *q );

const char *dfmt_historyhead( char *buf, unsigned int len );
const char *dfmt_history( char *buf, unsigned int len, duc_history *q );

const char *dfmt_taghead( char *buf, unsigned int len );
const char *dfmt_tag( char *buf, unsigned int len, duc_tag *q );

const char *dfmt_userhead( char *buf, unsigned int len );
const char *dfmt_user( char *buf, unsigned int len, duc_user *q );

const char *dfmt_albumhead( char *buf, unsigned int len );
const char *dfmt_album( char *buf, unsigned int len, duc_album *q );

const char *dfmt_artisthead( char *buf, unsigned int len );
const char *dfmt_artist( char *buf, unsigned int len, duc_artist *q );

const char *dfmt_sfilterhead( char *buf, unsigned int len );
const char *dfmt_sfilter( char *buf, unsigned int len, duc_sfilter *q );



#define BUFLENCLIENT 512
#define BUFLENTRACK 1024
#define BUFLENQUEUE 1024
#define BUFLENTAG 1024
#define BUFLENUSER 1024
#define BUFLENALBUM 1024
#define BUFLENARTIST 512
#define BUFLENSFILTER 4096

typedef int (*t_dmsg)( const char *fmt, va_list ap );

extern t_dmsg dmsg_msg_cb;
int dmsg_msg( const char *fmt, ... );

void dmsg_dump_clients( duc_it_client *it );
void dmsg_dump_tracks( duc_it_track *it );
void dmsg_dump_queue( duc_it_queue *it );
void dmsg_dump_history( duc_it_history *it );
void dmsg_dump_tags( duc_it_tag *it );
void dmsg_dump_users( duc_it_user *it );
void dmsg_dump_albums( duc_it_album *it );
void dmsg_dump_artists( duc_it_artist *it );
void dmsg_dump_sfilters( duc_it_sfilter *it );



int duc_cmd( dudlc *con, char *line );

typedef char *(*duc_cgen)( dudlc *con, const char *text, int state );
duc_cgen duc_cgen_find( dudlc *con, const char* line, unsigned int pos );

#endif

