#include <stdlib.h>

#include "dudlc/proto.h"
#include "dudlc/event.h"
#include "dudlc/player.h"

duc_playstatus duc_cmd_status( dudlc *c )
{
	return _duc_cmd_int(c, "status");
}

duc_track *duc_cmd_curtrack( dudlc *c )
{
	return _duc_cmd_conv(c, (_duc_converter) _duc_track_parse, 
			"curtrack");
}

int duc_cmd_play( dudlc *c )
{
	return _duc_cmd_succ(c, "play");
}

int duc_cmd_stop( dudlc *c )
{
	return _duc_cmd_succ(c, "stop");
}

int duc_cmd_next( dudlc *c )
{
	return _duc_cmd_succ(c, "next");
}

int duc_cmd_prev( dudlc *c )
{
	return _duc_cmd_succ(c, "prev");
}

int duc_cmd_pause( dudlc *c )
{
	return _duc_cmd_succ(c, "pause");
}


int duc_cmd_gap( dudlc *c )
{
	return _duc_cmd_int(c, "gap");
}

int duc_cmd_gapset( dudlc *c, unsigned int gap )
{
	return _duc_cmd_int(c, "gapset %u", gap );
}


int duc_cmd_random( dudlc *c )
{
	return _duc_cmd_int(c, "random");
}

int duc_cmd_randomset( dudlc *c, int r )
{
	return _duc_cmd_succ(c, "randomset %d", r );
}


static void _duc_bcast_newtrack( dudlc *c, const char *line )
{
	duc_track *t;
	char *end;

	if( NULL == (t= _duc_track_parse(line+4,&end)))
		return;

	_MSC_EVENT(c, nexttrack, c, t);

	duc_track_free(t);
}

void _duc_bcast_player( dudlc *c, const char *line )
{
	int r;

	switch(line[2]){
		case '0': /* newtrack */
			_duc_bcast_newtrack(c,line);
			break;

		case '1': /* stopped */
			_MSC_EVENT(c,stopped,c);
			break;

		case '2': /* paused */
			_MSC_EVENT(c,paused,c);
			break;

		case '3': /* resumed */
			_MSC_EVENT(c,resumed,c);
			break;

		case '6': /* random */
			r = atoi(line+4);
			_MSC_EVENT(c,random,c,r);
			break;


		default:
			_MSC_EVENT(c,bcast,c,line);
	}

	(void)c;
	(void)line;
}


