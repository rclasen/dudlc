#include <stdlib.h>
#include <string.h>

#include "dudlc/proto.h"
#include "dudlc/event.h"
#include "dudlc/random.h"

char *msc_cmd_filter( mservclient *c )
{
	return _msc_cmd_string(c, "filter" );
}

int msc_cmd_filterset( mservclient *c, const char *filter )
{
	return _msc_cmd_succ(c, "filterset %s", filter );
}

int msc_cmd_filterstat( mservclient *c )
{
	return _msc_cmd_int(c, "filterstat" );
}

msc_it_track *msc_cmd_randomtop( mservclient *c, int num )
{
	return _msc_iterate( c, (_msc_converter)_msc_track_parse, 
			"randomtop %d", num );
}

void _msc_bcast_random( mservclient *c, const char *line )
{
	switch(line[2]){
		case '0': /* filter changed */
			_MSC_EVENT(c,filter,c,line+4);
			break;

		case '1': /* sleeo changed - misplaced 621 */
			_MSC_EVENT(c,sleep,c,atoi(line+4));
			break;


		default:
			_MSC_EVENT(c,bcast,c,line);
	}
}


