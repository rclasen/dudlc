
#include "dudlc/event.h"
#include "dudlc/command.h"

msc_events *msc_getevents( mservclient *c )
{
	return c->ev;
}

msc_events *msc_setevents( mservclient *c, msc_events *e )
{
	msc_events *old;

	old = c->ev;
	c->ev = e;
	return old;
}

void _msc_bcast( mservclient *p, const char *line )
{
	switch(line[1]){
		case '3':
			_msc_bcast_client( p, line );
			break;

		case '4':
			_msc_bcast_player( p, line );
			break;
			
		case '5':
			_msc_bcast_random( p, line );
			break;
			
		case '6':
			_msc_bcast_queue( p, line );
			break;
			
		case '7':
			_msc_bcast_tag( p, line );
			break;
			
		default:
			_MSC_EVENT(p,bcast,p,line);
	}
}


