#include <stdlib.h>

#include <mservclient/proto.h>
#include <mservclient/sleep.h>

int msc_cmd_sleep( mservclient *c )
{
	if( _msc_cmd(c, "sleep" ))
		return -1;
	if( *_msc_rcode(c) != '2' )
		return -1;
	return atoi(_msc_rline(c));
}

int msc_cmd_sleepset( mservclient *c, int delay )
{
	if( _msc_cmd(c, "sleepset %d", delay ))
		return -1;
	if( *_msc_rcode(c) != '2' )
		return -1;
	return 0;
}

