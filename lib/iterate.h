#ifndef _MSERVCLIENT_ITERATE_H
#define _MSERVCLIENT_ITERATE_H

#include <mservclient/client.h>

typedef void *(*_msc_converter)( const char *line, char **end );

/* TODO: itarator */
typedef struct {
	mservclient *con;
	_msc_converter conv;
} _msc_iter;

_msc_iter *_msc_iterate( mservclient *p, _msc_converter conv, 
		const char *cmd, ... );
void *_msc_it_cur( _msc_iter *i );
void *_msc_it_next( _msc_iter *i );
void _msc_it_done( _msc_iter *i );




#endif
