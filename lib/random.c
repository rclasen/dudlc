#include <stdlib.h>
#include <string.h>

#include <mservclient/proto.h>
#include <mservclient/event.h>
#include <mservclient/random.h>

char *msc_cmd_filter( mservclient *c )
{
	if( _msc_cmd(c, "filter" ))
		return NULL;
	if( *_msc_rcode(c) != '2' )
		return NULL;
	return strdup( _msc_rline(c));
}

int msc_cmd_filterset( mservclient *c, const char *filter )
{
	if( _msc_cmd(c, "filterset %s", filter ))
		return -1;
	if( *_msc_rcode(c) != '2' )
		return -1;
	return 0;
}

int msc_cmd_filterstat( mservclient *c )
{
	if( _msc_cmd(c, "filterstat" ))
		return -1;
	if( *_msc_rcode(c) != '2' )
		return -1;
	return atoi(_msc_rline(c));
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


