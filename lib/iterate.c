
#include <stdlib.h>
#include <stdarg.h>

#include "dudlc/proto.h"
#include "dudlc/iterate.h"


_duc_iter *_duc_iterate( dudlc *p, _duc_converter conv, 
		const char *cmd, ... )
{
	va_list ap;
	int r;
	_duc_iter *it;

	if( ! conv )
		return NULL;

	va_start(ap, cmd );
	r = _duc_vsend( p, cmd, ap );
	va_end(ap);

	if( r )
		return NULL;

	if( _duc_rnext(p) )
		return NULL;

	if( _duc_rcode(p)[0] == '5' )
		goto clean1;

	if( NULL == (it = malloc(sizeof(_duc_iter))))
		goto clean1;

	it->con = p;
	it->conv = conv;

	return it;

clean1:
	_duc_rlast(p);
	return NULL;
}

void *_duc_it_cur( _duc_iter *i )
{
	if( ! i )
		return NULL;

	return (*i->conv)( _duc_rline(i->con), NULL );
}

void *_duc_it_next( _duc_iter *i )
{
	if( ! i )
		return NULL;

	if( _duc_rnext(i->con))
		return NULL;

	return _duc_it_cur( i );
}

void _duc_it_done( _duc_iter *i )
{
	if( ! i )
		return;

	_duc_rlast(i->con);
	free(i);
}



