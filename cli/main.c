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

#include "tty.h"
#include "events.h"
#include "options.h"
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

static void usage( void );


int main( int argc, char **argv )
{
	options cfg;
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

	options_init( &cfg );

	progname = argv[0];
	while( -1 != ( c = getopt_long( argc, argv, "hH:P:u:p:c:f:",
					lopts, NULL ))){
		switch( c ){
		  case 'h':
			  usage();
			  exit(0);
			  break;

		  case 'H':
			  cfg.host = strdup(optarg);
			  break;

		  case 'P':
			  cfg.port = atoi(optarg);
			  break;

		  case 'u':
			  cfg.user = strdup(optarg);
			  break;

		  case 'p':
			  cfg.pass = strdup(optarg);
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


	options_read( &cfg, cfname );

	con = duc_new( cfg.host, cfg.port );
	duc_setauth( con, cfg.user, cfg.pass );

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
