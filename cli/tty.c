
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

#include "command.h"
#include "main.h"
#include "tty.h"     

static int redisplay = 0;
static int inprompt = 1;


duc_cgen cgen = NULL;

static char *tty_cgen( const char *text, int state )
{
	if( NULL == cgen )
		return NULL;
	return( (*cgen)( con, text, state ));
}

/* handler invoked by readline to get a list of complettions */
static char **tty_completer( const char *text, int start, int end )
{
	duc_cgen gen;

	(void) end; /* shut up gcc */
	(void) start;

	rl_attempted_completion_over = 1;

	if( NULL == (gen = duc_cgen_find(con, rl_line_buffer, start )))
		return NULL;

	cgen = gen;
	return rl_completion_matches( text, tty_cgen );
}

static void cmd_user( char *input )
{
	char *pass;

	if( NULL == ( pass = getpass("Password:" ) )){
		tty_msg( "failed: %s\n", strerror(errno));
		return;
	}

	if( duc_setauth( con, input, pass )){
		tty_msg( "login failed\n" );
	} else {
		duc_close(con);
	}
}

/* handler invoked by readline when input is submitted with enter */
static void tty_executor( char *input )
{
	char *c;
	int len;

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
	for( c = input + strlen(input) - 1; c != input && isspace(*c); --c )
		*c = 0;

	add_history(input);

	/* execute action - if one is defined */
	inprompt = 0;

	len = strcspn( input, "\t " );
	if( 0 == strcasecmp( input, "quit" ) ){
		terminate++;

	} else if( 0 == strcasecmp( input, "exit" ) ){
		terminate++;

	} else if( len ==4 && 0 == strncasecmp( input, "user", 4 ) ){
		cmd_user( input +4 );

	} else {
		duc_cmd( con, input );
	}
	inprompt = 1;

	free(input);
}


void tty_init( const char *name, const char *prompt )
{
	rl_readline_name = name;
	rl_attempted_completion_function = tty_completer;
	
	rl_callback_handler_install( prompt, tty_executor );
}

void tty_redraw( void )
{
	if( redisplay ){
		rl_forced_update_display();
		redisplay = 0;
	}
}

void tty_poll( void )
{
	rl_callback_read_char();
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


