#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <mservclient/command.h>
#include <mservclient/proto.h>

#include "tty.h"
#include "mservcli.h"
#include "command.h"

#define BUFLENTRACK 1024

/************************************************************
 * helper
 */

static inline void trackhead( void  )
{
	tty_msg( "%5.5s %-20.20s %2.2s %-20.20s %s\n",
			"id", "album", "nr", "artist", "title" );
}

// TODO: lookup artist and album name
static inline const char *mktrack( char *buf, unsigned int len, msc_track *t )
{
	snprintf( buf, len, "%5d %-20d %2d %-20d %s",
			t->id, t->albumid, t->albumnr, t->artistid, t->title );

	return buf;
}

/************************************************************
 * sample command completer
 */

static char *cgen_cmd_foo( const char *line, int state )
{
	(void) line; /* shut up gcc */

	if( state == 0 )
		return strdup( "bar" );

	if( state == 1 )
		return strdup( "baz" );

	return 0;
}

#define CMD(n)		static void n(const char *line )

#define MSG_ARGMIS(a)	tty_msg( "missing argument: %s\n",a )
#define MSG_ARGMANY	tty_msg( "too many arguments\n" )
#define MSG_FAIL	\
	if( 0 > msc_fd(con) ){\
		tty_msg( "failed: connection error\n"); \
		return; \
	} else {\
		tty_msg( "failed: %s\n", msc_rmsg(con)); \
		return; \
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
		tty_msg( "disconnected %d", num );
		return;
	}
	MSG_FAIL;
}

/************************************************************
 * commands: user
 */

#ifdef todo_user
CMD(cmd_who)
{
	msc_it_client *it;
	msc_client *c;

	ARG_NONE;

	if( NULL == (it = msc_cmd_client(con))){
		MSG_FAIL;
	}
	for( c = msc_it_client_cur(it); c; c = msc_it_client_next(it)){
		tty_msg( "%s\n", mkclient(buf, c ));
		msc_client_free(c);
	}
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
		tty_msg( "kicked %d", num );
	}
	MSG_FAIL;
}
#endif



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
	}

	trackhead();
	tty_msg( "%s\n", mktrack(buf, BUFLENTRACK, t));
	msc_track_free(t);
}

CMD(cmd_gap)
{
	int gap;

	ARG_NONE;
	if( 0 > (gap = msc_cmd_gap(con))){
		MSG_FAIL;
	}

	tty_msg( "delay (gap) before starting next track: %d sec\n", gap );
}

#ifdef todo_player
CMD(cmd_gapset)
CMD(cmd_random)
CMD(cmd_randomset)
#endif

/************************************************************
 * commands: track
 */

CMD(cmd_tracks)
{
	int num;

	ARG_NONE;

	if( 0 > ( num = msc_cmd_tracks(con))){
		MSG_FAIL;
	}

	tty_msg( "tracks: %d\n", num );
}

CMD(cmd_trackget)
{
	int num;
	msc_track *t;
	char buf[BUFLENTRACK];

	ARG_NEED("trackID");

	num = atoi(line);
	if( NULL == ( t = msc_cmd_trackget(con, num ))){
		MSG_FAIL;
	}

	trackhead();
	tty_msg( "%s\n", mktrack(buf,BUFLENTRACK,t));
	msc_track_free( t );
}

static void dump_tracks( msc_it_track *it )
{
	msc_track *t;
	char buf[BUFLENTRACK];

	trackhead();
	for( t = msc_it_track_cur(it); t; t = msc_it_track_next(it)){
		tty_msg( "%s\n", mktrack(buf,BUFLENTRACK,t));
		msc_track_free(t);
	}
	msc_it_track_done(it);
}

CMD(cmd_tracksearch)
{
	msc_it_track *it;

	ARG_NEED("substr");

	if( NULL == (it = msc_cmd_tracksearch( con, line ))){
		MSG_FAIL;
	}

	dump_tracks(it);
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
	}

	dump_tracks(it);
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
	}

	dump_tracks(it);
}

// TODO: track_alter


/************************************************************
 * commands: misc
 */


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
	{ "foo", cgen_cmd_foo, NULL },
	{ "quit", NULL, cmd_quit },
	{ "help", NULL, cmd_help },
	{ "raw", NULL, cmd_raw },
	{ "open", NULL, cmd_open },
	{ "disconnect", NULL, cmd_disconnect },
	{ "play", NULL, cmd_play },
	{ "stop", NULL, cmd_stop },
	{ "next", NULL, cmd_next },
	{ "prev", NULL, cmd_prev },
	{ "pause", NULL, cmd_pause },
	{ "status", NULL, cmd_status },
	{ "curtrack", NULL, cmd_curtrack },
	{ "gap", NULL, cmd_gap },
	{ "tracks", NULL, cmd_tracks },
	{ "trackget", NULL, cmd_trackget },
	{ "tracksearch", NULL, cmd_tracksearch },
	{ "tracksalbum", NULL, cmd_tracksalbum },
	{ "tracksartist", NULL, cmd_tracksartist },

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

