
     
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
     
#include "tty.h"
#include "events.h"
#include "main.h"

dudlc *con = NULL;

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
			tty_redraw();
		}

		if( FD_ISSET(0,&rfds) ){
			tty_poll();
			tty_redraw();
		}

	} while( ! terminate );
}

int main( int argc, char **argv )
{
	char *user = "guest";
	char *pass = "guest";

	(void) argc;
	(void) argv;

	// TODO: getopt 

	dmsg_msg_cb = tty_vmsg;

	con = duc_new( "localhost", 4445 );
	duc_setauth( con, user, pass );

	events_init(con);
	
	signal( SIGTERM, sig_term );
	signal( SIGINT, sig_term );
	signal( SIGPIPE, SIG_IGN );

	tty_init( "dudlc", "> " );

	duc_open(con);

	loop();

	tty_done();

	tty_msg( "bye.\n" );
	return 0;
}


