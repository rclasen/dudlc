#include <stdlib.h>

#include <mservclient/proto.h>
#include <mservclient/event.h>
#include <mservclient/player.h>

msc_playstatus msc_cmd_status( mservclient *c )
{
	if( _msc_cmd(c, "status"))
		return -1;
	if( *_msc_rcode(c) != '2' )
		return -1;
	return atoi(_msc_rline(c));
}

msc_track *msc_cmd_curtrack( mservclient *c )
{
	if( _msc_cmd(c, "curtrack"))
		return NULL;
	if( *_msc_rcode(c) != '2' )
		return NULL;
	return _msc_track_parse(_msc_rline(c), NULL);
}

int msc_cmd_play( mservclient *c )
{
	if( _msc_cmd(c, "play"))
		return -1;
	if( *_msc_rcode(c) != '2' )
		return -1;
	return 0;
}

int msc_cmd_stop( mservclient *c )
{
	if( _msc_cmd(c, "stop"))
		return -1;
	if( *_msc_rcode(c) != '2' )
		return -1;
	return 0;
}

int msc_cmd_next( mservclient *c )
{
	if( _msc_cmd(c, "next"))
		return -1;
	if( *_msc_rcode(c) != '2' )
		return -1;
	return 0;
}

int msc_cmd_prev( mservclient *c )
{
	if( _msc_cmd(c, "prev"))
		return -1;
	if( *_msc_rcode(c) != '2' )
		return -1;
	return 0;
}

int msc_cmd_pause( mservclient *c )
{
	if( _msc_cmd(c, "pause"))
		return -1;
	if( *_msc_rcode(c) != '2' )
		return -1;
	return 0;
}


int msc_cmd_gap( mservclient *c )
{
	if( _msc_cmd(c, "gap"))
		return -1;
	if( *_msc_rcode(c) != '2' )
		return -1;
	return atoi(_msc_rline(c));
}

int msc_cmd_gapset( mservclient *c, unsigned int gap )
{
	if( _msc_cmd(c, "gapset %u", gap ))
		return -1;
	if( *_msc_rcode(c) != '2' )
		return -1;
	return 0;
}


int msc_cmd_random( mservclient *c )
{
	if( _msc_cmd(c, "random"))
		return -1;
	if( *_msc_rcode(c) != '2' )
		return -1;
	return atoi(_msc_rline(c));
}

int msc_cmd_randomset( mservclient *c, int r )
{
	if( _msc_cmd(c, "randomset %s", r ))
		return -1;
	if( *_msc_rcode(c) != '2' )
		return -1;
	return 0;
}


static void _msc_bcast_newtrack( mservclient *c, const char *line )
{
	msc_track *t;
	
	if( NULL == (t= _msc_track_parse(line+4,NULL)))
		return;

	_MSC_EVENT(c,nexttrack,c,t);
	msc_track_free(t);
}

void _msc_bcast_player( mservclient *c, const char *line )
{
	int r;

	switch(line[2]){
		case '0': /* newtrack */
			_msc_bcast_newtrack(c,line);
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


