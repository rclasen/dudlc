
#include <stdlib.h>
#include <stdarg.h>

#include <mservclient/proto.h>
#include <mservclient/iterate.h>


_msc_iter *_msc_iterate( mservclient *p, _msc_converter conv, 
		const char *cmd, ... )
{
	va_list ap;
	int r;
	_msc_iter *it;

	if( ! conv )
		return NULL;

	va_start(ap, cmd );
	r = _msc_vsend( p, cmd, ap );
	va_end(ap);

	if( r )
		return NULL;

	if( _msc_rnext(p) )
		return NULL;

	if( _msc_rcode(p)[0] == '5' )
		goto clean1;

	if( NULL == (it = malloc(sizeof(_msc_iter))))
		goto clean1;

	it->con = p;
	it->conv = conv;

	return it;

clean1:
	_msc_rlast(p);
	return NULL;
}

void *_msc_it_cur( _msc_iter *i )
{
	if( ! i )
		return NULL;

	return (*i->conv)( _msc_rline(i->con), NULL );
}

void *_msc_it_next( _msc_iter *i )
{
	if( ! i )
		return NULL;

	if( _msc_rnext(i->con))
		return NULL;

	return _msc_it_cur( i );
}

void _msc_it_done( _msc_iter *i )
{
	if( ! i )
		return;

	_msc_rlast(i->con);
	free(i);
}



