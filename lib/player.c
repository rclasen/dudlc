#include <stdlib.h>

#include <mservclient/proto.h>
#include <mservclient/event.h>
#include <mservclient/player.h>

msc_playstatus msc_cmd_status( mservclient *c )
{
	return _msc_cmd_int(c, "status");
}

msc_track *msc_cmd_curtrack( mservclient *c )
{
	return _msc_cmd_conv(c, (_msc_converter) _msc_track_parse, 
			"curtrack");
}

int msc_cmd_play( mservclient *c )
{
	return _msc_cmd_succ(c, "play");
}

int msc_cmd_stop( mservclient *c )
{
	return _msc_cmd_succ(c, "stop");
}

int msc_cmd_next( mservclient *c )
{
	return _msc_cmd_succ(c, "next");
}

int msc_cmd_prev( mservclient *c )
{
	return _msc_cmd_succ(c, "prev");
}

int msc_cmd_pause( mservclient *c )
{
	return _msc_cmd_succ(c, "pause");
}


int msc_cmd_gap( mservclient *c )
{
	return _msc_cmd_int(c, "gap");
}

int msc_cmd_gapset( mservclient *c, unsigned int gap )
{
	return _msc_cmd_int(c, "gapset %u", gap );
}


int msc_cmd_random( mservclient *c )
{
	return _msc_cmd_int(c, "random");
}

int msc_cmd_randomset( mservclient *c, int r )
{
	return _msc_cmd_succ(c, "randomset %d", r );
}


static void _msc_bcast_newtrack( mservclient *c, const char *line )
{
	msc_track *t;
	msc_artist *art;
	msc_album *alb;
	char *end;

	if( NULL == (t= _msc_track_parse(line+4,&end)))
		return;

	if( NULL == (art= _msc_artist_parse(end+1,&end)))
		goto clean2;

	if( NULL == (alb= _msc_album_parse(end+1,NULL)))
		goto clean3;

	_MSC_EVENT(c, nexttrack, c, t, art, alb);

	msc_album_free(alb);
clean3:
	msc_artist_free(art);
clean2:
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


