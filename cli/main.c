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


/* TODO: GPL license */

#define _GNU_SOURCE
     
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <pwd.h>
#include <glib.h>

#include "tty.h"
#include "events.h"
#include "main.h"

dudlc *con = NULL;
char *progname = NULL;

int terminate = 0;

static void sig_term( int sig )
{
	signal( sig, sig_term );
	terminate++;
}

static inline void largest( int *a, int b)
{
	if( *a < b )
		*a = b;
}

static void loop( void )
{
	fd_set rfds;
	int maxfd;
	int r;

	do {
		FD_ZERO(&rfds);
		maxfd = 0;

		FD_SET(0,&rfds);
		largest( &maxfd, 0 );

		if( -1 != duc_fd(con)){
			FD_SET( duc_fd(con), &rfds );
			largest( &maxfd, duc_fd(con));
		}

		maxfd++;

		if( 0 > ( r = select( maxfd, &rfds, NULL, NULL, NULL ))){
			if( errno == EINTR ){
				// interrupted by ^Z or SIGALRM...
				continue;
			}

			perror( "select" );
			exit( 1 );
		}
		
		if( -1 != duc_fd(con) && FD_ISSET(duc_fd(con), &rfds)){
			duc_poll(con);
		}

		if( FD_ISSET(0,&rfds) ){
			tty_poll();
		}

	} while( ! terminate );
}

static void def_string( char **dst, GKeyFile *kf, char *key, char *def )
{
	GError *err = NULL;

	if( *dst )
		return;

	if( kf )
		*dst = g_key_file_get_string( kf, "dudlc", key, &err );

	if( ! *dst )
		*dst = def;
}

static void def_integer( int *dst, GKeyFile *kf, char *key, int def )
{
	GError *err = NULL;

	if( *dst >= 0 )
		return;

	if( kf )
		*dst = g_key_file_get_integer( kf, "dudlc", key, &err );
	if( err && err->code == G_KEY_FILE_ERROR_INVALID_VALUE )
		fprintf( stderr, "invalid data for %s: %s", key, err->message );

	if( err || *dst < 0 )
		*dst = def;
}


static void usage( void );

int main( int argc, char **argv )
{
	char *host = NULL;
	int port = -1;
	char *user = NULL;
	char *pass = NULL;
	char *command = NULL;
	char *cfname = NULL;
	int c;
	int needhelp = 0;
	struct option lopts[] = {
		{"help", no_argument, NULL, 'h' },
		{"host", required_argument, NULL, 'H' },
		{"port", required_argument, NULL, 'P' },
		{"user", required_argument, NULL, 'u' },
		{"pass", required_argument, NULL, 'p' },
		{"command", required_argument, NULL, 'c' },
		{"config", required_argument, NULL, 'f' },
		{ 0,0,0,0 }
	};
	GKeyFile *keyfile = NULL;
	GError *err = NULL;

	progname = argv[0];
	while( -1 != ( c = getopt_long( argc, argv, "hH:P:u:p:c:f:",
					lopts, NULL ))){
		switch( c ){
		  case 'h':
			  usage();
			  exit(0);
			  break;

		  case 'H':
			  host = optarg;
			  break;

		  case 'P':
			  port = atoi(optarg);
			  break;

		  case 'u':
			  user = optarg;
			  break;

		  case 'p':
			  pass = optarg;
			  break;

		  case 'c':
			  command = optarg;
			  break;

		  case 'f':
			  cfname = optarg;
			  break;

		  default:
			  needhelp++;
		}
	}

	if( needhelp ){
		fprintf( stderr, "use --help for usage info\n" );
		exit(1);
	}

	if( ! cfname ){
		struct passwd *pw;
		if( NULL == (pw = getpwuid(getuid()))){
			fprintf( stderr, "failed to determin home\n");
		} else {
			asprintf( &cfname, "%s/.dudlc.conf", pw->pw_dir );
		}
	}

	keyfile = g_key_file_new();
	if( ! g_key_file_load_from_file( keyfile, cfname, G_KEY_FILE_NONE, &err )){
		if( err->domain == G_KEY_FILE_ERROR && 
				err->code == G_KEY_FILE_ERROR_NOT_FOUND )
			fprintf( stderr, "error reading config %s: %s", 
					cfname, err->message );
		g_key_file_free( keyfile );
		keyfile = NULL;
	}

	def_string( &host, keyfile, "host", "localhost" );
	def_integer( &port, keyfile, "port", 4445 );
	def_string( &user, keyfile, "user", "guest" );
	def_string( &pass, keyfile, "pass", "guest" );

	if( keyfile )
		g_key_file_free( keyfile );


	con = duc_new( host, port );
	duc_setauth( con, user, pass );


	if( command ){
		if( duc_open( con ) ){
			fprintf( stderr, "failed to connect: %s\n", 
					strerror(errno) );
			exit(1);
		}
		if( duc_cmd( con, command ) ){
			exit(1);
		}

	} else {
		signal( SIGTERM, sig_term );
		signal( SIGINT, SIG_IGN ); // TODO kill line on ^C
		signal( SIGPIPE, SIG_IGN );

		tty_init( progname, "> " );

		events_init(con);
		duc_open(con);

		loop();

		tty_done();
		tty_msg( "bye.\n" );
	}

	return 0;
}

static void usage( void )
{
	printf( "usage: %s [opts]\n", progname );
	printf( 
		" -h --help          this message\n"
		" -H --host <host>   host running dudld\n"
		" -P --port <port>   dudld port\n"
		" -u --user <name>   dudld user\n"
		" -p --pass <pass>   dudld password\n"
		" -c --command <cmd> run specified command and "
			"exit immediately\n"
	      );
}
