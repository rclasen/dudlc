
#ifndef _DUDLCCMD_H
#define _DUDLCCMD_H

#include <dudlc.h>

char *dfmt_bf;
char *dfmt_nf;

char *dfmt_clienthead( char *buf, unsigned int len );
char *dfmt_client( char *buf, unsigned int len, duc_client *c );

char *dfmt_trackid( int albumid, int nr );

char *dfmt_trackhead( char *buf, unsigned int len );
char *dfmt_track( char *buf, unsigned int len, duc_track *t );

char *dfmt_queuehead( char *buf, unsigned int len );
char *dfmt_queue( char *buf, unsigned int len, duc_queue *q );

char *dfmt_historyhead( char *buf, unsigned int len );
char *dfmt_history( char *buf, unsigned int len, duc_history *q );

char *dfmt_taghead( char *buf, unsigned int len );
char *dfmt_tag( char *buf, unsigned int len, duc_tag *q );

char *dfmt_userhead( char *buf, unsigned int len );
char *dfmt_user( char *buf, unsigned int len, duc_user *q );

char *dfmt_albumhead( char *buf, unsigned int len );
char *dfmt_album( char *buf, unsigned int len, duc_album *q );

char *dfmt_artisthead( char *buf, unsigned int len );
char *dfmt_artist( char *buf, unsigned int len, duc_artist *q );

char *dfmt_sfilterhead( char *buf, unsigned int len );
char *dfmt_sfilter( char *buf, unsigned int len, duc_sfilter *q );



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

void dmsg_dump_client( duc_client *t );
void dmsg_dump_clients( duc_it_client *it );

void dmsg_dump_track( duc_track *t );
void dmsg_dump_tracks( duc_it_track *it );

void dmsg_dump_queue( duc_queue *t );
void dmsg_dump_queues( duc_it_queue *it );

void dmsg_dump_history( duc_history *t );
void dmsg_dump_historys( duc_it_history *it );

void dmsg_dump_tag( duc_tag *t );
void dmsg_dump_tags( duc_it_tag *it );

void dmsg_dump_user( duc_user *t );
void dmsg_dump_users( duc_it_user *it );

void dmsg_dump_album( duc_album *t );
void dmsg_dump_albums( duc_it_album *it );

void dmsg_dump_artist( duc_artist *t );
void dmsg_dump_artists( duc_it_artist *it );

void dmsg_dump_sfilter( duc_sfilter *t );
void dmsg_dump_sfilters( duc_it_sfilter *it );




int duc_cmd( dudlc *con, char *line );

typedef char *(*duc_cgen)( dudlc *con, const char *text, int state );
duc_cgen duc_cgen_find( dudlc *con, char* line, unsigned int pos );

#endif

