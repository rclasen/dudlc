#ifndef _FORMATTER_H
#define _FORMATTER_H

#define BUFLENTRACK 1024

#include <mservclient/command.h>


const char *mktrackhead( char *buf, unsigned int len );
const char *mktrack( char *buf, unsigned int len, msc_track *t );
void dump_tracks( msc_it_track *it );

#endif
