
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

#include <lirc/lirc_client.h>
#include <dudlc.h>
#include <dudlccmd.h>

int dobeep = 0;
dudlc *dudl;

/*
 * print debugging message
 */
#define DPRINT( fmt... ) \
	syslog( LOG_DEBUG, "DEBUG: " fmt )

#define syslog_perror( msg ) \
	syslog( LOG_ERR, "%s: %s", msg, strerror(errno) )

#define eprintf( msg ) \
	fprintf( stderr, "%s: %s\n", progname, msg );

static void usage( char *progname, FILE *out );

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
 * execute lirc cmds as long as lircd is alive
 */
static void loop_lirccmd( struct lirc_config *config )
{
	char *code;
	char *c;
	int ret;

	while (lirc_nextcode(&code) == 0) {
		if (code == NULL)
			continue;

		DPRINT( "loop_lirccmd(): got code" );
		while ((ret = lirc_code2char(config, code, &c)) == 0 &&
		    c != NULL) {

			beep();

			if( duc_open( dudl ) ) {
				syslog( LOG_ERR, "reconnect to dudld failed" );
				continue;
			}

			if( duc_cmd( dudl, c ) ){
				syslog( LOG_ERR, "duc_cmd failed for %s", c);
				continue;
			}

			beep();
		}
		free(code);

		if (ret == -1)
			break;
	}
}


/* 
 * (re-)connect to lircd and execute commands
 */
static void loop_lirc( char *conf, 
		char *progname, 
		int reconnect )
{
	struct lirc_config *config;

	while(1){
		DPRINT( "loop_lirc() starting loop" );
		if (lirc_init(progname, 1) == -1){
			syslog( LOG_WARNING, "can't connect to lirc" );
			goto cont;
		}

		if (lirc_readconfig( conf, &config, NULL) != 0) {
			syslog( LOG_WARNING, "can't read lirc config '%s'",
				conf );
			lirc_deinit();
			goto cont;
		}

		loop_lirccmd( config );

		lirc_freeconfig(config);

		lirc_deinit();

cont:
		DPRINT( "waiting %d seconds before reconnect", reconnect );
		sleep( reconnect );
	}
}

int main(int argc, char *argv[])
{
	char *progname;
	int needhelp = 0;
	int debug = 0;
	char *conf = NULL;
	int foreground = 0;
	char *host = NULL;
	int port = 0;
	char *user = NULL;
	char *pass = NULL;

	progname = strrchr( argv[0], '/' );
	if( NULL != progname ){
		progname++;
	} else {
		progname = argv[0];
	}

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
			{0, 0, 0, 0}
		};
		c = getopt_long(argc, argv, "bhfDH:P:u:p:", long_options, NULL);
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
		conf = argv[optind];
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

	/* initialize dudl connection */
	dudl = duc_new( host, port );
	if( dudl == NULL ){
		eprintf( "could not init dudl connection" );
		exit( EXIT_FAILURE );
	}
	duc_setauth( dudl, user, pass );
	duc_open( dudl );


	/* start work */
	if (!foreground && daemon(0, 0) == -1) {
		eprintf( "can't daemonize" );
		syslog_perror(progname);
		exit(EXIT_FAILURE);
	}
	
	loop_lirc( conf, progname, 60 );



	/* cleanup */
	duc_free( dudl );

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
		);
}

