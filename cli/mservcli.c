
     
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
     
#include <mservclient/command.h>

#include "tty.h"

mservclient *con = NULL;

msc_events cb;

static int terminate = 0;

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

		if( -1 != msc_fd(con)){
			FD_SET( msc_fd(con), &rfds );
			largest( &maxfd, msc_fd(con));
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
		
		if( -1 != msc_fd(con) && FD_ISSET(msc_fd(con), &rfds)){
			msc_poll(con);
			tty_redraw();
		}

		if( FD_ISSET(0,&rfds) ){
			if( tty_poll() )
				terminate ++;
			tty_redraw();
		}

	} while( ! terminate );
}

static void bcast( mservclient *c, const char *line )
{
	tty_msg( "%s\n", line );
	(void)c;
}

int main( int argc, char **argv )
{

	(void) argc;
	(void) argv;

	// TODO: getopt 

	con = msc_new( "localhost", 4445, "ddd", "sss" );

	memset(&cb, 0, sizeof(cb));
	cb.bcast = bcast;
	msc_setevents( con, &cb );

	
	signal( SIGTERM, sig_term );
	signal( SIGINT, sig_term );
	signal( SIGPIPE, SIG_IGN );

	tty_init( "hmserv", "> " );

	loop();

	tty_done();

	tty_msg( "bye.\n" );
	return 0;
}


