
#include <errno.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <syslog.h>
#include <fcntl.h>
#include <signal.h>

#include <libncc/pidfile.h>

#include <lirc/lirc_client.h>
#include <dudlc.h>
#include <dudlccmd.h>

char *progname = NULL;
char *lircfile = NULL;
int dobeep = 0;

dudlc *dudl = NULL;
int reread_config = 0;


/*
 * print debugging message
 */
#define DPRINT( fmt... ) \
	syslog( LOG_DEBUG, "DEBUG: " fmt )

#define eprintf( msg ) \
	fprintf( stderr, "%s: %s\n", progname, msg );

static void usage( char *progname, FILE *out );

static void sig_hup( int sig )
{
	signal( sig, sig_hup );
	reread_config++;
}

static void beep( void )
{
	FILE *con;

	if( ! dobeep )
		return;

	con = fopen( "/dev/console", "a" );
	if( con == NULL ){
		return;
	}
	fprintf( con, "\a" );
	fclose( con );
}

/*
 * retry a command to allow reconnect
 */
static int dudl_cmd( dudlc *con, char *cmd )
{
	int rv;

	rv = duc_cmd(con, cmd );

	if( rv != -EIO )
		return rv;
	
	DPRINT("retrying command" );
	return duc_cmd(con, cmd );
}

/* 
 * execute lirc cmds as long as lircd is alive
 */
static void loop_lirccmd( struct lirc_config *config, char *code )
{
	char *c;

	/* lirc_code2char analyzes the string passed to it in the
	 * second argument and sets the third argument to the
	 * argument from the config.
	 */
	while ( !lirc_code2char(config, code, &c) && c != NULL ){

		beep();

		if( dudl_cmd( dudl, c ) ){
			syslog( LOG_ERR, "duc_cmd failed for %s", c);
			continue;
		}

		beep();
	}
}

static inline void setmax( int *max, int b )
{
	if( *max < b )
		*max = b;
}

/* 
 * (re-)connect to lircd and execute commands
 */
static void loop_lirc( void )
{
	int l_fd = -1;
	struct lirc_config *l_conf = NULL;

	while(1){
		int maxfd;
		fd_set rfd;
		struct timeval tv;
		struct timeval *ptv;

		/* re- open lirc connection */
		if( -1 == l_fd ){
			syslog(LOG_NOTICE, "(re-)connecting to lircd" );
			if( -1 == (l_fd = lirc_init(progname, 1) )){
				syslog( LOG_ERR, "can't connect to lirc" );
			} else {
				if( -1 == fcntl( l_fd, F_SETFL, O_NONBLOCK ))
					syslog( LOG_ERR, "fcntl failed: %m" );
			}
		}

		/* re- read lirc config */
		if( ! l_conf || reread_config ){
			syslog(LOG_NOTICE, "(re-)reading config %s", lircfile );
			if( l_conf ){
				lirc_freeconfig( l_conf );
				l_conf = NULL;
			}

			if( lirc_readconfig( lircfile, &l_conf, NULL) ){
				syslog( LOG_ERR, "can't read "
						"lirc config '%s'", lircfile );
			} else {
				reread_config = 0;
			}
		}

		/* re- connect to server */
		duc_open(dudl);

		/* wait for input */

		maxfd = 0;
		FD_ZERO( &rfd );

		if( -1 != l_fd ){
			setmax( &maxfd, l_fd );
			FD_SET( l_fd, &rfd );
		}

		if( -1 != duc_fd( dudl )){
			setmax( &maxfd, duc_fd( dudl ) );
			FD_SET( duc_fd( dudl ), &rfd );
		}

		maxfd++;

		/* set timeout for reconnect */
		tv.tv_usec = 0;
		tv.tv_sec = 60;
		ptv = ( l_fd == -1 || -1 == duc_fd( dudl )) ? &tv : NULL;

		if( 0 > select( maxfd, &rfd, NULL, NULL, ptv )){
			if( errno == EINTR )
				continue;

			syslog( LOG_CRIT, "select failed: %m" );
			exit(1);
		}

		/* process dudl input before reconnect can occur */
		if( -1 != duc_fd( dudl ) && FD_ISSET( duc_fd( dudl ), &rfd ))
			duc_poll( dudl );

		/* process LIRC input */
		if( -1 != l_fd && FD_ISSET( l_fd, &rfd ) ){
			char *code = NULL;

			/* lirc_nextcode tries to fetch a complete line
			 * from lircd. The line is returned in the first
			 * argument.
			 *
			 * it returns 0 on success and -1 on failure.
			 * errno is not set by it - so it should be
			 * untouched on failures
			 *
			 * it handles EAGAIN properly - the lirc fd can be
			 * set to non-blocking.
			 */
			if( lirc_nextcode(&code) ) {
				lirc_deinit();
				l_fd = -1;

			} else if( code && l_conf ){
				DPRINT( "got lirc input..." );
				loop_lirccmd( l_conf, code );
			}

			free(code);
		}
	}

	if( -1 != l_fd )
		lirc_deinit();
	if( l_conf )
		lirc_freeconfig( l_conf );
}

int main(int argc, char *argv[])
{
	int pid;
	int needhelp = 0;
	int debug = 0;
	int foreground = 0;
	char *host = NULL;
	int port = 0;
	char *user = NULL;
	char *pass = NULL;
	int pidfile = 0;

	progname = strrchr( argv[0], '/' );
	if( NULL != progname ){
		progname++;
	} else {
		progname = argv[0];
	}
	pid = getpid();

	while (1) {
		int c;
		static struct option long_options[] = {
			{"help", no_argument, NULL, 'h'},
			{"foreground", no_argument, NULL, 'f'},
			{"debug", no_argument, NULL, 'D'},
			{"beep", no_argument, NULL, 'b'},
			{"host", required_argument, NULL, 'H'},
			{"port", required_argument, NULL, 'P'},
			{"user", required_argument, NULL, 'u'},
			{"pass", required_argument, NULL, 'p'},
			{"beep", no_argument, NULL, 'b'},
			{"pidfile", no_argument, NULL, 'l'},
			{0, 0, 0, 0}
		};
		c = getopt_long(argc, argv, "bhfDlH:P:u:p:", 
				long_options, NULL);
		if (c == -1)
			break;
		switch (c) {
		case 'h':
			usage( progname, stdout );
			return (EXIT_SUCCESS);
		case 'f':
			foreground = 1;
			break;
		case 'D':
			debug++;
			break;
		case 'b':
			dobeep ++;
			break;
		case 'H':
			host = strdup(optarg);
			break;
		case 'P':
			port = atoi(optarg);
			break;
		case 'u':
			user = strdup(optarg);
			break;
		case 'p':
			pass = strdup(optarg);
			break;
		case 'l':
			pidfile++;
			break;
		default:
			needhelp++;
		}
	}
	if (optind < argc - 1) {
		eprintf( "too many arguments");
		needhelp++;
	}
	if( needhelp ){
		usage( progname, stderr );
		exit( EXIT_FAILURE );
	}

	if( optind != argc ){
		lircfile = argv[optind];
	}

	if( ! lircfile ){
		eprintf( "missing config file" );
		exit( EXIT_FAILURE );
	}


	/* set defaults */
	if( host == NULL ){
		host = "localhost";
	}

	if( port == 0 ){
		port = 4445;
	}

	if( user == NULL ){
		user = progname;
	}

	if( pass == NULL ){
		pass = "";
	}


	openlog( progname, LOG_PID, LOG_DAEMON );
	if( ! debug ){
		setlogmask( LOG_UPTO(LOG_INFO) );
	}

	if( pidfile && pidfile_lock(progname)){
		syslog( LOG_ERR, "cannot create pidfile: %m"); 
		exit( EXIT_FAILURE );
	}

	/* initialize dudl connection */
	dudl = duc_new( host, port );
	if( dudl == NULL ){
		eprintf( "could not init dudl connection" );
		exit( EXIT_FAILURE );
	}
	duc_setauth( dudl, user, pass );


	/* start work */
	if (!foreground && daemon(0, 0) == -1) {
		eprintf( "can't daemonize" );
		syslog(LOG_ERR, "daemonize failed: %m" );
		exit(EXIT_FAILURE);
	}

	if( pidfile && pidfile_take(progname, pid)){
		syslog( LOG_ERR, "cannot update pidfile: %m"); 
		exit( EXIT_FAILURE );
	}

	signal( SIGHUP, sig_hup );
	signal( SIGPIPE, SIG_IGN );

	loop_lirc();



	/* cleanup */
	duc_free( dudl );
	pidfile_unlock( progname );

	exit(EXIT_SUCCESS);
}

void usage( char *progname, FILE *out )
{
	fprintf(out, "Usage: %s [options] [config_file]\n", progname);
	fprintf(out,
		"   -h --help        display this message\n"
		"   -f --foreground  don't detach\n"
		"   -D --debug       enable debuging output\n"
		"   -H --host <h>    Host running dudld\n"
		"   -P --port <p>    dudld port\n"
		"   -u --user <u>    dudld user\n"
		"   -p --pass <p>    dudld password\n"
		"   -l --pidfile     create and check a pidfile\n"
		"   -b --beep        enable beeps\n"
		);
}

