#ifndef _FORMATTER_H
#define _FORMATTER_H

#define BUFLENCLIENT 512
#define BUFLENTRACK 1024
#define BUFLENQUEUE 1024
#define BUFLENTAG 1024
#define BUFLENUSER 1024

#include <dudlc.h>

const char *mkclienthead( char *buf, unsigned int len );
const char *mkclient( char *buf, unsigned int len, duc_client *c );
void dump_clients( duc_it_client *it );

const char *mktrackid( int albumid, int nr );

const char *mkrtrack( char *buf, unsigned int len, 
		duc_track *t, duc_artist *ar, duc_album *al );
const char *mktrackhead( char *buf, unsigned int len );
const char *mktrack( char *buf, unsigned int len, duc_track *t );
void dump_tracks( duc_it_track *it );

const char *mkqueuehead( char *buf, unsigned int len );
const char *mkqueue( char *buf, unsigned int len, duc_queue *q );
void dump_queue( duc_it_queue *it );

const char *mkhistoryhead( char *buf, unsigned int len );
const char *mkhistory( char *buf, unsigned int len, duc_history *q );
void dump_history( duc_it_history *it );

const char *mktaghead( char *buf, unsigned int len );
const char *mktag( char *buf, unsigned int len, duc_tag *q );
void dump_tags( duc_it_tag *it );

const char *mkuserhead( char *buf, unsigned int len );
const char *mkuser( char *buf, unsigned int len, duc_user *q );
void dump_users( duc_it_user *it );


#endif
