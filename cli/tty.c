
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
     
#include <readline/readline.h>
#include <readline/history.h>

#include <mservclient/command.h>
#include <mservclient/proto.h>

#include "tty.h"     

extern mservclient *con;

/* types */

typedef char *(*t_generator)( const char *text, int state );
typedef void (*t_action)( const char *text );

typedef struct _t_command {
	char *name;
	t_generator gen;
	t_action action;
} t_command;


static int redisplay = 0;
static int terminate = 0;
static int inprompt = 1;

/************************************************************
 * command completer
 */

static char *gencomp_cmd_foo( const char *text, int state )
{
	(void) text; /* shut up gcc */

	if( state == 0 )
		return strdup( "bar" );

	if( state == 1 )
		return strdup( "baz" );

	return 0;
}


/************************************************************
 * command actions
 */

static void cmd_quit( const char *text )
{
	(void) text; /* shut up gcc */
	terminate++;
}

static void cmd_tracks( const char *text )
{
	int num;

	if( *text ){
		tty_msg( "invalid arguments\n" );
		return;
	}

	if( 0 > ( num = msc_cmd_tracks(con))){
		tty_msg( "failed: %s", msc_rmsg(con));
		return;
	}

	tty_msg( "tracks: %d\n", num );
}

static const char *mktrack( char *buf, int len, msc_track *t )
{
	snprintf( buf, len, "%5d %4d %2d %4d %s",
			t->id, t->albumid, t->albumnr, t->artistid, t->title );

	return buf;
}

static void cmd_trackget( const char *text )
{
	int num;
	msc_track *t;
	char buf[1024];

	if( ! *text ){
		tty_msg( "missing arguments\n" );
		return;
	}

	num = atoi(text);
	if( NULL == ( t = msc_cmd_track_get(con, num ))){
		tty_msg( "failed: %s", msc_rmsg(con));
		return;
	}

	tty_msg( "%s\n", mktrack(buf,1024,t));
	msc_track_free( t );
}

static void cmd_tracksearch( const char *text )
{
	msc_it_track *it;
	msc_track *t;
	char buf[1024];

	if( ! *text ){
		tty_msg( "missing arguments\n" );
		return;
	}

	if( NULL == (it = msc_cmd_tracks_search( con, text ))){
		tty_msg( "failed: %s", msc_rmsg(con));
		return;
	}

	for( t = msc_it_track_cur(it); t; t = msc_it_track_next(it)){
		tty_msg( "%s\n", mktrack(buf,1024,t));
		msc_track_free(t);
	}
	msc_it_track_done(it);
}

static void cmd_misc( const char *text )
{
	_msc_send( con, text );
	if( _msc_rnext(con) ){
		tty_msg( "connection error\n" );
		return;
	}

	tty_msg( "code: %s\n", _msc_rcode(con));
	do {
		tty_msg( " %s\n", _msc_rline(con));
	} while( ! _msc_rnext(con));
}

/************************************************************
 * command list
 */

static t_command commands[] = {
	{ "quit", NULL, cmd_quit },
	{ "help", NULL, NULL },
	{ "foo", gencomp_cmd_foo, NULL },
	{ "tracks", NULL, cmd_tracks },
	{ "trackget", NULL, cmd_trackget },
	{ "tracksearch", NULL, cmd_tracksearch },

	{ NULL, NULL, NULL }
};


static t_command *command_nfind( const char *cmd, int len )
{
	int c;

	for( c = 0; commands[c].name; ++c ){
		if( 0 == strncasecmp(cmd, commands[c].name, len )){
			return &commands[c];
		}
	}

	return NULL;
}

static inline int command_len( const char *cmd )
{
	return strcspn(cmd, "\t ");
}


static t_command *command_find( const char *cmd )
{
	int len;

	if( 0 >= (len = command_len(cmd)))
		return NULL;

	return command_nfind( cmd, len );
}

static char *gencomp_cmd( const char *text, int state )
{
	static int c;
	static int len;
	char *name;

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

/* handler invoked by readline to get a list of complettions */
static char **hmserv_complete( const char *text, int start, int end )
{
	t_command *cmd;

	(void) end; /* shut up gcc */

	rl_attempted_completion_over = 1;

	/* complete a command name */
	if( start == 0 )
		return rl_completion_matches( text, gencomp_cmd );
	
	/* complete command specific arguments */
	if( NULL != (cmd = command_find( rl_line_buffer )) && cmd->gen )
		return rl_completion_matches( text, cmd->gen );

	return NULL;
}

/* handler invoked by readline when input is submitted with enter */
static void command_handler( char *input )
{
	int c;
	t_command *cmd;

	/* readline got EOF */
	if( ! input ){
		terminate++;
		return;
	}

	/* empty command */
	if( ! *input ){
		free( input );
		return;
	}

	/* strip trailing whitespace */
	for( c = strlen(input)-1; c >= 0 && isspace(input[c]); --c )
		input[c] = 0;

	add_history(input);

	/* execute action - if one is defined */
	inprompt = 0;
	c = command_len(input);
	if( NULL != (cmd = command_nfind( input, c )) && cmd->action )
		cmd->action( input+c );
	else 
		cmd_misc( input );
	inprompt = 1;

	free(input);
}


void tty_init( const char *name, const char *prompt )
{
	rl_readline_name = name;
	rl_attempted_completion_function = hmserv_complete;
	
	rl_callback_handler_install( prompt, command_handler );
}

void tty_redraw( void )
{
	if( redisplay ){
		rl_forced_update_display();
		redisplay = 0;
	}
}

int tty_poll( void )
{
	rl_callback_read_char();

	return terminate;
}

int tty_vmsg( const char *fmt, va_list ap )
{
	int r;

	if( ! redisplay && inprompt )
		printf( "\n" );

	r = vprintf( fmt, ap );
	if( inprompt )
		redisplay++;

	return r;
}

int tty_msg( const char * fmt, ... )
{
	va_list ap;
	int r;

	va_start( ap, fmt );
	r = tty_vmsg( fmt, ap );
	va_end( ap );

	return r;
}

void tty_done( void )
{
	rl_callback_handler_remove();
}


