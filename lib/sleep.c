#include <stdlib.h>

#include "dudlc/proto.h"
#include "dudlc/sleep.h"

int msc_cmd_sleep( mservclient *c )
{
	return _msc_cmd_int(c, "sleep" );
}

int msc_cmd_sleepset( mservclient *c, int delay )
{
	return _msc_cmd_succ(c, "sleepset %d", delay );
}

