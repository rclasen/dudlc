#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <mservclient/command.h>
#include <mservclient/proto.h>

#include "tty.h"
#include "mservcli.h"
#include "formatter.h"
#include "command.h"

static int atobool( const char *in )
{
	if( 0 == strcasecmp( in, "on" ) ){
		return 1;
	} 
	
	if( 0 == strcasecmp( in, "off" )){
		return 0;
	} 
	
	if( 0 == strcasecmp( in, "true" )){
		return 1;
	} 
	
	if( 0 == strcasecmp( in, "false" )){
		return 0;
	} 
	
	if( 0 == strcasecmp( in , "yes" )){
		return 1;
	} 
	
	if( 0 == strcasecmp( in , "no" )){
		return 0;
	} 
	
	if( 0 == strcmp( in, "1" )){
		return 1;
	} 
	
	if( 0 == strcmp( in, "0" )){
		return 0;
	} 

	return -1;
}

static int trackid( const char *in, char **end )
{
	int a;
	int b;
	char *s, *e;

	if( end ) (const char*) *end = in;

	a = strtol( in, &e, 10 );
	if( e == in )
		return -1;

	if( *e != '/' ){
		if( end ) *end = e;
		return a;
	}

	s = e + 1;
	b = strtol( s, &e, 10 );
	if( e == s )
		return -1;

	if( end ) *end = e;
	return msc_cmd_trackid(con, a, b );
}

#define CMD(n)		static void n(const char *line )

#define MSG_BADARG(a)	tty_msg( "bad argument for argument %s\n", a)
#define MSG_ARGMIS(a)	tty_msg( "missing argument: %s\n",a )
#define MSG_ARGMANY	tty_msg( "too many arguments\n" )
#define MSG_FAIL	\
	if( 0 > msc_fd(con) ){\
		tty_msg( "failed: connection error\n"); \
	} else {\
		tty_msg( "failed: %s\n", msc_rmsg(con)); \
	}


#define ARG_NEED(a) \
	if( ! *line ){\
		MSG_ARGMIS(a);\
		return;\
	}
#define ARG_NONE \
	if( *line ){\
		MSG_ARGMANY;\
		return;\
	}

/************************************************************
 * command actions
 */

CMD(cmd_quit)
{
	(void) line; /* shut up gcc */
	terminate++;
}

CMD(cmd_open)
{
	ARG_NONE;
	msc_open( con );
}

static char *cgen_cmd_raw( const char *text, int state )
{
	static char **helpv = NULL;
	static int last = 0;

	if( state == 0 ){
		msc_it_help *it;
		char *s;
		unsigned int len;
		int total = 0;
		int used = 0;

		len = strlen(text);

		it = msc_cmd_help(con);
		for( s = msc_it_help_cur(it); s; 
				s = msc_it_help_next(it) ){

			if( 0 != strncasecmp( s, text, len) ){
				free(s);
				continue;
			}

			if( used >= total ){
				char **tmp;

				total += 10;
				tmp = realloc( helpv, sizeof(char*) * 
						(total +1) );
				if( tmp == NULL ){
					free(s);
					break;
				}

				helpv = tmp;
			}

			helpv[used++] = s;
		}
		msc_it_help_done(it);

		if( helpv ){
			helpv[used] = NULL;
			last = 0;
			return *helpv;
		
		}

		last = 0;
		return NULL;
	}

	if( ! helpv )
		return NULL;

	if( ! helpv[++last] ){
		free(helpv);
		helpv = NULL;
		last = 0;
		return NULL;
	}

	return helpv[last];
}



CMD(cmd_help)
{
	ARG_NONE;
	// TODO: remote help
	tty_msg( "not yet working\n" );
}

CMD(cmd_raw)
{
	ARG_NEED("raw_command" );

	// TODO: make this a lib function
	_msc_send( con, line );
	if( _msc_rnext(con) ){
		tty_msg( "connection error\n" );
		return;
	}

	tty_msg( "code: %s\n", _msc_rcode(con));
	do {
		tty_msg( " %s\n", _msc_rline(con));
	} while( ! _msc_rnext(con));
}

// TODO: client ID completer using who output
CMD(cmd_disconnect)
{
	int num;

	ARG_NEED("conID");

	num = atoi(line);
	if( 0 == msc_cmd_disconnect( con, num ) ){
		tty_msg( "disconnected client #%d\n", num );
		return;
	}
	MSG_FAIL;
}

/************************************************************
 * commands: user
 */

CMD(cmd_who)
{
	msc_it_client *it;

	ARG_NONE;

	if( NULL == (it = msc_cmd_who(con))){
		MSG_FAIL;
		return;
	}
	dump_clients(it);
	msc_it_client_done(it);
}

// TODO: user ID completer using who output
// TODO: take username as argument, too
CMD(cmd_kick)
{
	int num;

	ARG_NEED("userID");

	num = atoi(line);
	if( 0 == msc_cmd_kick( con, num ) ){
		tty_msg( "kicked user #%d\n", num );
	}
	MSG_FAIL;
}



/************************************************************
 * commands: player
 */

CMD(cmd_play)
{
	ARG_NONE;
	if( msc_cmd_play(con) ){
		MSG_FAIL;
	}
}

CMD(cmd_stop)
{
	ARG_NONE;
	if( msc_cmd_stop(con) ){
		MSG_FAIL;
	}
}

CMD(cmd_next)
{
	ARG_NONE;
	if( msc_cmd_next(con) ){
		MSG_FAIL;
	}
}

CMD(cmd_prev)
{
	ARG_NONE;
	if( msc_cmd_prev(con) ){
		MSG_FAIL;
	}
}

CMD(cmd_pause)
{
	ARG_NONE;
	if( msc_cmd_pause(con) ){
		MSG_FAIL;
	}
}

CMD(cmd_status)
{
	int stat;

	ARG_NONE;
	if( 0 > ( stat = msc_cmd_status(con))){
		MSG_FAIL;
		return;
	}
	switch(stat){
		case 0:
			tty_msg("stopped\n");
			break;

		case 1:
			tty_msg("playing\n");
			break;

		case 2:
			tty_msg("paused\n");
			break;

	}
}

CMD(cmd_curtrack)
{
	msc_track *t;
	char buf[BUFLENTRACK];

	ARG_NONE;
	if( NULL == (t = msc_cmd_curtrack(con))){
		MSG_FAIL;
		return;
	}

	tty_msg( "%s\n\n", mktrackhead(buf, BUFLENTRACK));
	tty_msg( "%s\n", mktrack(buf, BUFLENTRACK, t));
	msc_track_free(t);
}

CMD(cmd_gap)
{
	int gap;

	ARG_NONE;
	if( 0 > (gap = msc_cmd_gap(con))){
		MSG_FAIL;
		return;
	}

	tty_msg( "delay (gap) before starting next track: %d sec\n", gap );
}

CMD(cmd_gapset)
{
	int gap;
	char *end;

	ARG_NEED("seconds");

	gap = strtol(line, &end, 10 );
	if( *end ){
		MSG_BADARG("seconds");
		return;
	}

	if( msc_cmd_gapset(con, gap )){
		MSG_FAIL;
		return;
	}

	tty_msg( "delay (gap) set to %d sec\n", gap );
}

CMD(cmd_random)
{
	int random;

	ARG_NONE;
	if( 0 > (random = msc_cmd_random(con))){
		MSG_FAIL;
		return;
	}

	tty_msg( "random play (when queue is empty) is %s\n", 
			random ? "on" : "off" );
}

CMD(cmd_randomset)
{
	int random = 0;

	ARG_NEED("random_status");

	if( -1 == ( random = atobool( line ))){
		MSG_BADARG("random_status");
		return;
	}

	if( msc_cmd_randomset(con, random)){
		MSG_FAIL;
	}
}

/************************************************************
 * commands: track
 */

CMD(cmd_tracks)
{
	int num;

	ARG_NONE;

	if( 0 > ( num = msc_cmd_tracks(con))){
		MSG_FAIL;
		return;
	}

	tty_msg( "available tracks: %d\n", num );
}

CMD(cmd_trackid)
{
	char *s;
	char *e;
	int albumid;
	int num;
	int id;

	(const char *)s = line;
	albumid = strtol(s, &e, 10 );
	if( s == e ){
		MSG_BADARG( "albumID" );
		return;
	}

	s = e + strspn(e, "\t /");
	num = strtol(s, &e, 10 );
	if( s==e || *e ){
		MSG_BADARG( "trackNr" );
		return;
	}

	if( 0 > (id = msc_cmd_trackid( con, albumid, num))){
		tty_msg( "track not found" );
		return;
	}

	tty_msg( "track %s has ID: %d\n", mktrackid(albumid, num), id );
}


CMD(cmd_trackget)
{
	int num;
	msc_track *t;
	char buf[BUFLENTRACK];

	ARG_NEED("trackID");

	if( 0 > (num = trackid( line, NULL ))){
		MSG_BADARG( "trackID" );
	}
	if( NULL == ( t = msc_cmd_trackget(con, num ))){
		MSG_FAIL;
		return;
	}

	tty_msg( "%s\n\n", mktrackhead(buf, BUFLENTRACK));
	tty_msg( "%s\n", mktrack(buf,BUFLENTRACK,t));
	msc_track_free( t );
}

CMD(cmd_tracksearch)
{
	msc_it_track *it;

	ARG_NEED("substr");

	if( NULL == (it = msc_cmd_tracksearch( con, line ))){
		MSG_FAIL;
		return;
	}

	dump_tracks(it);
	msc_it_track_done(it);
}

// TODO: use album name
// TODO: album name completer
CMD(cmd_tracksalbum)
{
	int id;
	msc_it_track *it;

	ARG_NEED("albumID");

	id = atoi(line);
	if( NULL == (it = msc_cmd_tracksalbum( con, id ))){
		MSG_FAIL;
		return;
	}

	dump_tracks(it);
	msc_it_track_done(it);
}

// TODO: use artist name
// TODO: artist name completer
CMD(cmd_tracksartist)
{
	int id;
	msc_it_track *it;

	ARG_NEED("artistID");

	id = atoi(line);
	if( NULL == (it = msc_cmd_tracksartist( con, id ))){
		MSG_FAIL;
		return;
	}

	dump_tracks(it);
	msc_it_track_done(it);
}

// TODO: track_alter


/************************************************************
 * commands: random/filter
 */

CMD(cmd_filter)
{
	char *f;

	ARG_NONE;

	if( NULL == (f = msc_cmd_filter( con ))){
		MSG_FAIL;
		return;
	}

	tty_msg( "current filter: %s\n", f );
	free(f);
}

CMD(cmd_filterset)
{
	if( msc_cmd_filterset(con, line)){
		MSG_FAIL;
	}
}

CMD(cmd_filterstat)
{
	int n;

	ARG_NONE;
	if( 0 > ( n = msc_cmd_filterstat(con))){
		MSG_FAIL;
		return;
	}

	tty_msg( "tracks matching filter: %d\n", n );
}

CMD(cmd_randomtop)
{
	int num = 20;
	char *end;
	msc_it_track *it;

	if( *line ){
		num = strtol(line, &end, 10 );
		if( *end ){
			MSG_BADARG("maxnumber");
			return;
		}
	}

	if( NULL == (it = msc_cmd_randomtop( con, num ))){
		MSG_FAIL;
		return;
	}

	dump_tracks(it);
	msc_it_track_done(it);
}


/************************************************************
 * commands: queue 
 */

CMD(cmd_queue)
{
	msc_it_queue *it;

	ARG_NONE;

	if( NULL == (it = msc_cmd_queue( con ))){
		MSG_FAIL;
		return;
	}
	dump_queue(it);
	msc_it_queue_done(it);
}

CMD(cmd_queueget)
{
	char buf[BUFLENTRACK];
	int id;
	msc_queue *q;

	ARG_NEED("queueID");
	id = atoi(line);
	if( NULL == (q = msc_cmd_queueget(con, id))){
		MSG_FAIL;
		return;
	}

	tty_msg("%s\n\n", mkqueuehead(buf, BUFLENTRACK));
	tty_msg("%s\n", mkqueue(buf, BUFLENTRACK,q));
	msc_queue_free(q);
}

// TODO: queue a specified list of tracks
CMD(cmd_queueadd)
{
	int id;

	ARG_NEED("trackID");

	if( 0 > (id = trackid(line, NULL))){
		MSG_BADARG("trackID");
		return;
	}

	if( 0 > msc_cmd_queueadd(con, id)){
		MSG_FAIL;
	}
}

// TODO: queue a specified list of albums
// TODO: queue current album
CMD(cmd_queuealbum)
{
	int id;

	ARG_NEED("albumID");

	if( 0 > (id = trackid(line, NULL))){
		MSG_BADARG("trackID");
		return;
	}

	if( 0 > msc_cmd_queuealbum(con, id)){
		MSG_FAIL;
	}
}


// TODO: unqueue range, by user, client, album, track
// TODO: unqueue a list
CMD(cmd_queuedel)
{
	int id;

	ARG_NEED("queueID");
	id = atoi(line);
	if( msc_cmd_queuedel(con, id )){
		MSG_FAIL;
	}
}

CMD(cmd_queueclear)
{
	ARG_NONE;

	if( msc_cmd_queueclear( con )){
		MSG_FAIL;
	}
}

/************************************************************
 * commands: sleep
 */

CMD(cmd_sleep)
{
	int r;
	ARG_NONE;

	if( 0 > ( r = msc_cmd_sleep( con ))){
		MSG_FAIL;
		return;
	}

	tty_msg( "falling asleep in %d sek\n", r );
}

CMD(cmd_sleepset)
{
	int t;
	char *end;

	ARG_NEED("seconds");
	t = strtol(line, &end, 10 );
	if( *end ){
		MSG_BADARG("seconds");
		return;
	}

	if( msc_cmd_sleepset(con, t )){
		MSG_FAIL;
	}
}


/************************************************************
 * commands: history
 */

CMD(cmd_history)
{
	int num = 20;
	char *end;
	msc_it_history *it;

	if( *line ){
		num = strtol(line, &end, 10 );
		if( *end ){
			MSG_BADARG("maxnumber");
			return;
		}
	}

	it = msc_cmd_history(con, num );
	dump_history(it);
	msc_it_history_done(it);
}

CMD(cmd_historytrack)
{
	int num = 20;
	int id;
	msc_it_history *it;
	char *end;

	if( 0 > (id = trackid(line, &end))){
		MSG_BADARG("trackID");
		return;
	}

	if( *end ){
		num = strtol(end, &end, 10 );
		if( *end ){
			MSG_BADARG("maxnumber");
			return;
		}
	}

	it = msc_cmd_historytrack(con, id, num );
	dump_history(it);
	msc_it_history_done(it);
}

/************************************************************
 * commands: tag
 */

CMD(cmd_taglist)
{
	msc_it_tag *it;

	ARG_NONE;
	it = msc_cmd_taglist( con );
	dump_tags( it );
	msc_it_tag_done( it );
}

CMD(cmd_tagget)
{
	int id;
	char *e;
	msc_tag *t;
	char buf[BUFLENTAG];

	ARG_NEED("tagID");
	id = strtol( line, &e, 10 );
	if( *e ){
		MSG_BADARG("tagID");
		return;
	}

	if( NULL == (t= msc_cmd_tagget(con, id))){
		MSG_FAIL;
		return;
	}

	tty_msg( "%s\n\n", mktaghead(buf,BUFLENTAG));
	tty_msg( "%s\n", mktag(buf,BUFLENTAG, t));

	msc_tag_free(t);
}

CMD(cmd_tagname)
{
	msc_tag *t;
	char buf[BUFLENTAG];

	ARG_NEED("tagName");
	if( NULL == (t= msc_cmd_tagname(con, line))){
		MSG_FAIL;
		return;
	}

	tty_msg( "%s\n\n", mktaghead(buf,BUFLENTAG));
	tty_msg( "%s\n", mktag(buf,BUFLENTAG, t));

	msc_tag_free(t);
}

CMD(cmd_tagadd)
{
	int id;

	ARG_NEED( "tagName" );
	if( 0 > (id = msc_cmd_tagadd( con, line ))){
		MSG_FAIL;
		return;
	}

	tty_msg( "added with id %d\n", id );
}

CMD(cmd_tagsetname)
{
	char *e;
	int id;


	ARG_NEED("tagID");
	id = strtol( line, &e, 10 );
	if( line == e ){
		MSG_BADARG("tagID");
		return;
	}

	e += strspn(e, "\t ");
	if( msc_cmd_tagsetname(con, id, e)){
		MSG_FAIL;
	}
}

CMD(cmd_tagsetdesc)
{
	char *e;
	int id;


	ARG_NEED("tagID");
	id = strtol( line, &e, 10 );
	if( line == e ){
		MSG_BADARG("tagID");
		return;
	}

	e += strspn(e, "\t ");
	if( msc_cmd_tagsetdesc(con, id, e)){
		MSG_FAIL;
	}
}

CMD(cmd_tagdel)
{
	int id;
	char *e;

	ARG_NEED("tagID");
	id = strtol( line, &e, 10 );
	if( *e ){
		MSG_BADARG("tagID");
		return;
	}

	if( msc_cmd_tagdel(con, id)){
		MSG_FAIL;
	}
}

CMD(cmd_tracktags)
{
	int id;
	char *e;
	msc_it_tag *it;

	ARG_NEED("trackID");
	id = strtol( line, &e, 10 );
	if( *e ){
		MSG_BADARG("trackID");
		return;
	}

	it = msc_cmd_tracktags( con, id );
	dump_tags( it );
	msc_it_tag_done( it );
}

CMD(cmd_tracktagged)
{
	int tagid, trid;
	char *s, *e;
	int r;

	ARG_NEED("trackID");
	trid = trackid( line, &e );
	if( e == line ){
		MSG_BADARG("trackID");
		return;
	}

	s = e += strspn( e, "\t " );
	tagid = strtol(s, &e, 10 );
	if( *e ){
		MSG_BADARG("trackID");
		return;
	}

	if( 0 > (r = msc_cmd_tracktagged( con, trid, tagid ))){
		MSG_FAIL;
		return;
	}

	tty_msg( "tag is%s set for this track\n", r ? "": " not" );
}

CMD(cmd_tracktagset)
{
	int tagid, trid;
	char *s, *e;

	ARG_NEED("trackID");
	trid = trackid( line, &e );
	if( e == line ){
		MSG_BADARG("trackID");
		return;
	}

	s = e += strspn( e, "\t " );
	tagid = strtol(s, &e, 10 );
	if( *e ){
		MSG_BADARG("trackID");
		return;
	}

	if( msc_cmd_tracktagset( con, trid, tagid )){
		MSG_FAIL;
		return;
	}
	tty_msg( "set tag\n");
}

CMD(cmd_tracktagdel)
{
	int tagid, trid;
	char *s, *e;

	ARG_NEED("trackID");
	trid = trackid( line, &e );
	if( e == line ){
		MSG_BADARG("trackID");
		return;
	}

	s = e += strspn( e, "\t " );
	tagid = strtol(s, &e, 10 );
	if( *e ){
		MSG_BADARG("trackID");
		return;
	}

	if( msc_cmd_tracktagdel( con, trid, tagid )){
		MSG_FAIL;
		return;
	}

	tty_msg( "tag deleted\n");
}


/************************************************************
 * command list
 */

typedef void (*t_action)( const char *line );

typedef struct _t_command {
	char *name;
	t_generator gen;
	t_action action;
} t_command;

static t_command commands[] = {
	{ "quit", NULL, cmd_quit },
	{ "exit", NULL, cmd_quit },
	{ "help", NULL, cmd_help },
	{ "raw", cgen_cmd_raw, cmd_raw },
	{ "open", NULL, cmd_open },
	{ "disconnect", NULL, cmd_disconnect },
	{ "who", NULL, cmd_who },
	{ "kick", NULL, cmd_kick },
	{ "play", NULL, cmd_play },
	{ "stop", NULL, cmd_stop },
	{ "next", NULL, cmd_next },
	{ "prev", NULL, cmd_prev },
	{ "pause", NULL, cmd_pause },
	{ "status", NULL, cmd_status },
	{ "curtrack", NULL, cmd_curtrack },
	{ "gap", NULL, cmd_gap },
	{ "gapset", NULL, cmd_gapset },
	{ "random", NULL, cmd_random },
	{ "randomset", NULL, cmd_randomset },
	{ "tracks", NULL, cmd_tracks },
	{ "trackid", NULL, cmd_trackid },
	{ "trackget", NULL, cmd_trackget },
	{ "tracksearch", NULL, cmd_tracksearch },
	{ "tracksalbum", NULL, cmd_tracksalbum },
	{ "tracksartist", NULL, cmd_tracksartist },
	{ "filter", NULL, cmd_filter },
	{ "filterset", NULL, cmd_filterset },
	{ "filterstat", NULL, cmd_filterstat },
	{ "randomtop", NULL, cmd_randomtop },
	{ "queue", NULL, cmd_queue },
	{ "queueget", NULL, cmd_queueget },
	{ "queueadd", NULL, cmd_queueadd },
	{ "queuealbum", NULL, cmd_queuealbum },
	{ "queuedel", NULL, cmd_queuedel },
	{ "queueclear", NULL, cmd_queueclear },
	{ "sleep", NULL, cmd_sleep },
	{ "sleepset", NULL, cmd_sleepset },
	{ "history", NULL, cmd_history },
	{ "historytrack", NULL, cmd_historytrack },
	{ "taglist", NULL, cmd_taglist },
	{ "tagget", NULL, cmd_tagget },
	{ "tagname", NULL, cmd_tagname },
	{ "tagadd", NULL, cmd_tagadd },
	{ "tagsetname", NULL, cmd_tagsetname },
	{ "tagsetdesc", NULL, cmd_tagsetdesc },
	{ "tagdel", NULL, cmd_tagdel },
	{ "tracktags", NULL, cmd_tracktags },
	{ "tracktagged", NULL, cmd_tracktagged },
	{ "tracktagset", NULL, cmd_tracktagset },
	{ "tracktagdel", NULL, cmd_tracktagdel },

	{ NULL, NULL, NULL }
};


/* toplevel completer */
static char *cgen_top( const char *text, int state )
{
	static int c;
	static unsigned int len;
	char *name;

	/* TODO use "help" */
	if( state == 0 ){
		c = 0;
		len = strlen(text);
	}

	while( NULL != (name = commands[c].name) ){
		c++;
		
		if( strncasecmp(text,name,len) == 0 )
			return strdup(name);
	}

	return NULL;
}

static t_command *command_nfind( const char *cmd, unsigned int len )
{
	int c;

	for( c = 0; commands[c].name; ++c ){
		if( len != strlen(commands[c].name))
			continue;

		if( 0 == strncasecmp(cmd, commands[c].name, len )){
			return &commands[c];
		}
	}

	return NULL;
}

#define WSPACE	"\t "

t_generator command_completer( const char *line, unsigned int pos )
{
	int len;
	t_command *cmd;

	/* skip leading whitespace */
	len = strspn( line, WSPACE );
	line += len;
	pos -= len;

	/* begin of line -> complete the command name */
	if( pos == 0 )
		return cgen_top;

	/* else try to complete command arguments */
	// TODO: make completer finding more elegant
	len = strcspn( line, WSPACE );
	if( NULL != (cmd = command_nfind( line, len )))
		return cmd->gen;

	return NULL;
}

void command_action( const char *line )
{
	int len;
	int skip;
	t_command *cmd;

	/* skip leading whitespace */
	line += strspn( line, WSPACE );

	len = strcspn( line, WSPACE );
	if( NULL != (cmd = command_nfind( line, len ))){
		skip = strspn( line + len, WSPACE );
		if( cmd->action )
			cmd->action( line + len + skip );

		return;
	}

	tty_msg( "no such command - try to send it with 'raw'?\n" );
}

