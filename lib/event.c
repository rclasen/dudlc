
#include <mservclient/event.h>

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


