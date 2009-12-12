/*
 * Copyright (c) 2008 Rainer Clasen
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * The GNU General Public License is often shipped with GNU software, and
 * is generally kept in a file called COPYING or LICENSE.  If you do not
 * have a copy of the license, write to the Free Software Foundation,
 * 59 Temple Place, Suite 330, Boston, MA 02111 USA.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <term.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "command.h"
#include "main.h"
#include "tty.h"

static char tent[4096];
static struct {
	char *clreol;
	char *abold;
	char *anorm;
} tcap = {
	.clreol = "",
	.abold = "",
	.anorm = "",
};

/*
 * pointer function that builds the list of completions for current
 * context
 */
duc_cgen cgen = NULL;

static char *topcmds[] = {
	"quit",
	"exit",
	"user",
	NULL
};

/*
 * called by readline to get one/next possible completion
 * for top-level commands.
 *
 * this adds local completions to the ones returned from the lib.
 */
static char *tty_cgen_top( const char *text, int state )
{
	static char **next;
	static unsigned int len;

	if( state == 0 ){
		next = topcmds;
		len = strlen(text);
	}

	if( cgen ){
		char *r;

		if( NULL != (r = (*cgen)( con, text, state )))
			return r;
	}

	while( *next ){
		if( strncasecmp(text,*next,len) == 0 )
			return strdup(*(next++));
		next++;
	}
	return NULL;
}

/*
 * called by readline to get one/next possible completion
 * for commands arguments.
 */
static char *tty_cgen( const char *text, int state )
{
	if( ! cgen )
		return NULL;

	return (*cgen)( con, text, state );
}

/*
 * handler invoked by readline to get a list of complettions
 */
static char **tty_completer( const char *text, int start, int end )
{
	duc_cgen gen;

	(void) end; /* shut up gcc */

	rl_attempted_completion_over = 1;

	if( NULL == (gen = duc_cgen_find(con, rl_line_buffer, start )))
		return NULL;

	cgen = gen;

	return rl_completion_matches( text, start ? tty_cgen : tty_cgen_top );
}

static void cmd_user( char *input )
{
	char *pass;

	if( ! *input ){
		tty_msg( "missing username\n");
		return;
	}

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

	free(input);
}


void tty_init( const char *name, const char *prompt )
{

	if( isatty(STDOUT_FILENO)){
		char *term;
		char *area = tent;

		if( NULL == (term = getenv("TERM") )){
			term="dumb";
		}

		if( 0 <= tgetent( tent, term ) ){
			tcap.clreol = tgetstr("ce", &area );
			tcap.abold = tgetstr("md", &area );
			tcap.anorm = tgetstr("me", &area );
		}
	}

	dmsg_msg_cb = tty_vmsg;
	dfmt_bf = tcap.abold;
	dfmt_nf = tcap.anorm;

	rl_readline_name = name;
	rl_attempted_completion_function = tty_completer;

	rl_callback_handler_install( prompt, tty_executor );
}

void tty_redraw( void )
{
	rl_on_new_line();
	rl_redisplay();
}

void tty_poll( void )
{
	rl_callback_read_char();
}

int tty_vmsg( const char *fmt, va_list ap )
{
	int r;

	putchar(13);
	putp( tcap.clreol );
	r = vprintf( fmt, ap );
	rl_on_new_line();

	if(! RL_ISSTATE(RL_STATE_DONE) )
		rl_redisplay();

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


