
     
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
     
#include "input.h"

static int terminate = 0;

static void sig_term( int sig )
{
	signal( sig, sig_term );
	terminate++;
}

int main( int argc, char **argv )
{
	fd_set rfds, wfds;
	struct timeval tv;
	int maxfd;
	int r;

	(void) argc;
	(void) argv;

	// TODO: getopt 

	signal( SIGTERM, sig_term );
	signal( SIGINT, sig_term );
	signal( SIGPIPE, SIG_IGN );

	tty_init( "hmserv", "> " );

	do {
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		FD_SET(0,&rfds); // watch stdin
		maxfd = 1;

		tv.tv_sec = 2;
		tv.tv_usec = 0;

		if( 0 > ( r = select( maxfd, &rfds, &wfds, NULL, &tv ))){
			if( errno == EINTR ){
				// interrupted by ^Z or SIGALRM...
				continue;
			}

			perror( "select" );
			exit( 1 );
		}
		
		if( r == 0 ){
			tty_msg(".\n");
		}

		if( FD_ISSET(0,&rfds) && tty_poll() )
			terminate ++;

	} while( ! terminate );

	tty_done();

	tty_msg( "bye.\n" );
	return 0;
}


