
     
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
     
#include "proto.h"
#include "tty.h"

t_proto *con = NULL;

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

int main( int argc, char **argv )
{
	fd_set rfds;
	int maxfd;
	int r;

	(void) argc;
	(void) argv;

	// TODO: getopt 

	con = proto_new( "localhost", 4445, "ddd", "sss" );
	
	signal( SIGTERM, sig_term );
	signal( SIGINT, sig_term );
	signal( SIGPIPE, SIG_IGN );

	tty_init( "hmserv", "> " );

	do {
		FD_ZERO(&rfds);
		maxfd = 0;

		FD_SET(0,&rfds);
		largest( &maxfd, 0 );

		if( -1 != proto_fd(con)){
			FD_SET( proto_fd(con), &rfds );
			largest( &maxfd, proto_fd(con));
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
		
		if( FD_ISSET(0,&rfds) && tty_poll() )
			terminate ++;

		if( -1 != proto_fd(con) && FD_ISSET(proto_fd(con), &rfds))
			proto_poll(con);

	} while( ! terminate );

	tty_done();

	tty_msg( "bye.\n" );
	return 0;
}


