#ifndef _FORMATTER_H
#define _FORMATTER_H

#define BUFLENCLIENT 512
#define BUFLENTRACK 1024
#define BUFLENQUEUE 1024
#define BUFLENTAG 1024

#include <mservclient/command.h>

const char *mkclienthead( char *buf, unsigned int len );
const char *mkclient( char *buf, unsigned int len, msc_client *c );
void dump_clients( msc_it_client *it );

const char *mktrackid( int albumid, int nr );

const char *mktrackhead( char *buf, unsigned int len );
const char *mktrack( char *buf, unsigned int len, msc_track *t );
void dump_tracks( msc_it_track *it );

const char *mkqueuehead( char *buf, unsigned int len );
const char *mkqueue( char *buf, unsigned int len, msc_queue *q );
void dump_queue( msc_it_queue *it );

const char *mkhistoryhead( char *buf, unsigned int len );
const char *mkhistory( char *buf, unsigned int len, msc_history *q );
void dump_history( msc_it_history *it );

const char *mktaghead( char *buf, unsigned int len );
const char *mktag( char *buf, unsigned int len, msc_tag *q );
void dump_tags( msc_it_tag *it );


#endif
