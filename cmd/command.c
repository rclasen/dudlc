#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include <dudlc.h>
#include <dudlc/proto.h>

#include "dudlccmd.h"

typedef int (*t_action)( dudlc *dudl, const char *line );

static int atobool( const char *in )
{
	if( 0 == strcasecmp( in, "on" ) ){
		return 1;
	} 
	
	if( 0 == strcasecmp( in, "off" )){
		return 0;
	} 
	
	if( 0 == strcasecmp( in, "true" )){
		return 1;
	} 
	
	if( 0 == strcasecmp( in, "false" )){
		return 0;
	} 
	
	if( 0 == strcasecmp( in , "yes" )){
		return 1;
	} 
	
	if( 0 == strcasecmp( in , "no" )){
		return 0;
	} 
	
	if( 0 == strcmp( in, "1" )){
		return 1;
	} 
	
	if( 0 == strcmp( in, "0" )){
		return 0;
	} 

	return -1;
}

static int track2id( dudlc *con, const char *in, char **end )
{
	int a;
	int b;
	char *s, *e;
	duc_track *t;

	if( end ) (const char*) *end = in;

	if( *in == 'c' ){
		if(NULL == (t = duc_cmd_curtrack(con))){
			return -1;
		}
		a = t->id;
		duc_track_free(t);

		if( end ) (*end)++;
		return a;
	}

	a = strtol( in, &e, 10 );
	if( e == in )
		return -1;

	if( *e != '/' ){
		if( end ) *end = e;
		return a;
	}

	s = e + 1;
	b = strtol( s, &e, 10 );
	if( e == s )
		return -1;

	if( end ) *end = e;
	return duc_cmd_track2id(con, a, b );
}

static int tag2id( dudlc *con, const char *in, char **end )
{
	int id;
	char *e;
	char tag[10];
	int len;

	if( end ) (const char*) *end = in;

	id = strtol( in, &e, 10 );
	if( in != e ){
		if( end ) *end = e;
		return id;
	}

	len = strcspn(in, "\t ");
	if( len >= 10  )
		return -1;

	(const char*) e = in +len;
	strncpy( tag, in, len );
	if( 0 > ( id = duc_cmd_tag2id( con, tag ))){
		return -1;
	}

	if( end ) *end = e;
	return id;
}

// TODO: ask server for user2id
static int user2id( dudlc *con, const char *in, char **end )
{
	(void) con;
	return strtol( in, end, 10 );
}

// TODO: ask server for right2id
static int right2id( dudlc *con, const char *in, char **end )
{
	(void) con;
	return strtol( in, end, 10 );
}


#define CMD(n)	static int n(dudlc *con, const char *line )
#define CGEN(n)	static char *n( dudlc *con, const char *text, int state )

#define CMD_FAIL_BADARG(a)	{ \
	dmsg_msg( "bad argument for argument %s\n", a); \
	return(-EINVAL); }
#define CMD_FAIL_ARGMIS(a)	{ \
	dmsg_msg( "missing argument: %s\n",a ); \
	return(-EINVAL); }
#define CMD_FAIL_ARGMANY	{ \
	dmsg_msg( "too many arguments\n" ); \
	return(-EINVAL); }

#define CMD_OK	return 0
#define CMD_FAIL	{ \
	if( 0 > duc_fd(con) ){\
		dmsg_msg( "failed: connection error\n"); \
		return( -EIO ); \
	} else {\
		dmsg_msg( "failed: %s\n", duc_rmsg(con)); \
		return( -ENOEXEC ); \
	} }

#define ARG_NEED(a) \
	if( ! *line ){\
		CMD_FAIL_ARGMIS(a);\
	}
#define ARG_NONE \
	if( *line ){\
		CMD_FAIL_ARGMANY;\
	}

/************************************************************
 * command actions
 */

CMD(cmd_open)
{
	(void) con;

	ARG_NONE;
	if( duc_open( con ) )
		CMD_FAIL;

	CMD_OK;
}


CMD(cmd_help)
{
	(void) con;

	ARG_NONE;
	// TODO: ask server for CMD help

	dmsg_msg( "not yet implemented\n" );
	return -ENOTSUP;
}

CMD(cmd_raw)
{
	(void) con;

	ARG_NEED("raw_command" );

	// TODO: make cmd_raw a lib function
	_duc_send( con, line );
	if( _duc_rnext(con) )
		CMD_FAIL;

	dmsg_msg( "code: %s\n", _duc_rcode(con));
	do {
		dmsg_msg( " %s\n", _duc_rline(con));
	} while( ! _duc_rnext(con));

	CMD_OK;
}

CGEN(cgen_raw)
{
	static char **helpv = NULL;
	static int last = 0;

	if( state == 0 ){
		duc_it_help *it;
		char *s;
		unsigned int len;
		int total = 0;
		int used = 0;

		len = strlen(text);

		it = duc_cmd_help(con);
		for( s = duc_it_help_cur(it); s; 
				s = duc_it_help_next(it) ){

			if( 0 != strncasecmp( s, text, len) ){
				free(s);
				continue;
			}

			if( used >= total ){
				char **tmp;

				total += 10;
				tmp = realloc( helpv, sizeof(char*) * 
						(total +1) );
				if( tmp == NULL ){
					free(s);
					break;
				}

				helpv = tmp;
			}

			helpv[used++] = s;
		}
		duc_it_help_done(it);

		if( helpv ){
			helpv[used] = NULL;
			last = 0;
			return *helpv;
		
		}

		last = 0;
		return NULL;
	}

	if( ! helpv )
		return NULL;

	if( ! helpv[++last] ){
		free(helpv);
		helpv = NULL;
		last = 0;
		return NULL;
	}

	return helpv[last];
}


/************************************************************
 * commands: user
 */

CMD(cmd_clientlist)
{
	duc_it_client *it;

	ARG_NONE;

	if( NULL == (it = duc_cmd_clientlist(con)))
		CMD_FAIL;

	dmsg_dump_clients(it);
	duc_it_client_done(it);

	CMD_OK;
}

// TODO: client ID completer using who output
CMD(cmd_clientclose)
{
	int num;

	ARG_NEED("conID");

	num = atoi(line);
	if( duc_cmd_clientclose( con, num ) )
		CMD_FAIL;

	dmsg_msg( "disconnected client #%d\n", num );
	CMD_OK;
}

// TODO: user ID completer using who output
CMD(cmd_clientcloseuser)
{
	int uid;
	char *end;

	ARG_NEED("userID");

	uid = user2id(con, line, &end);
	if( *end )
		CMD_FAIL_ARGMIS("uid");

	if( duc_cmd_clientcloseuser( con, uid ) )
		CMD_FAIL;

	dmsg_msg( "kicked user #%d\n", uid );
	CMD_OK;
}

// TODO: remove user2id cmd
CMD(cmd_user2id)
{
	int uid;
	ARG_NEED("name");

	if( 0 > ( uid = duc_cmd_usergetname(con, line))){
		CMD_FAIL;
	}

	dmsg_msg( "user has ID %d\n", uid );
	CMD_OK;
}

CMD(cmd_userget)
{
	int uid;
	duc_user *u;
	char buf[BUFLENUSER];
	char *end;

	ARG_NEED("uid");

	uid = user2id( con, line, &end );
	if( *end )
		CMD_FAIL_ARGMIS("uid");

	if( NULL == ( u = duc_cmd_userget( con, uid )))
		CMD_FAIL;

	dmsg_msg( "%s\n\n", dfmt_userhead(buf, BUFLENUSER));
	dmsg_msg( "%s\n", dfmt_user(buf, BUFLENUSER, u));

	duc_user_free(u);
	CMD_OK;
}

CMD(cmd_userlist)
{
	duc_it_user *it;

	(void)line;
	ARG_NONE;

	if( NULL == (it = duc_cmd_userlist( con )))
		CMD_FAIL;

	dmsg_dump_users(it);
	duc_it_user_done(it);
	CMD_OK;
}

CMD(cmd_usersetpass)
{
	int uid;
	char *end;

	ARG_NEED("uid");

	uid = user2id( con, line, &end );
	if( line == end )
		CMD_FAIL_ARGMIS("uid");

	end += strspn(end, "\t ");
	if( duc_cmd_usersetpass(con, uid, end ))
		CMD_FAIL;

	dmsg_msg( "password changed\n");
	CMD_OK;
}

CMD(cmd_usersetright)
{
	int uid;
	int right;
	char *end, *s;

	ARG_NEED("uid");

	uid = user2id( con, line, &end );
	if( line == end )
		CMD_FAIL_ARGMIS("uid");

	s = end + strspn( end, "\t ");
	right = right2id( con, s, &end );
	if( *end )
		CMD_FAIL_ARGMIS("right");

	if( duc_cmd_usersetright( con, uid, right ))
		CMD_FAIL;

	dmsg_msg( "right changed\n");
	CMD_OK;
}

CMD(cmd_useradd)
{
	int uid;
	ARG_NEED("name");

	if( 0 > (uid = duc_cmd_useradd(con, line)))
		CMD_FAIL;

	dmsg_msg( "user added with id %d\n", uid);
	CMD_OK;
}

CMD(cmd_userdel)
{
	int uid;
	char *end;

	ARG_NEED("uid");

	uid = user2id( con, line, &end );
	if( *end )
		CMD_FAIL_ARGMIS("uid");

	if( duc_cmd_userdel(con, uid))
		CMD_FAIL;

	dmsg_msg("user deleted\n");
	CMD_OK;
}


/************************************************************
 * commands: player
 */

CMD(cmd_play)
{
	ARG_NONE;
	if( duc_cmd_play(con) )
		CMD_FAIL;

	CMD_OK;
}

CMD(cmd_stop)
{
	ARG_NONE;
	if( duc_cmd_stop(con) )
		CMD_FAIL;

	CMD_OK;
}

CMD(cmd_next)
{
	ARG_NONE;
	if( duc_cmd_next(con) )
		CMD_FAIL;

	CMD_OK;
}

CMD(cmd_prev)
{
	ARG_NONE;
	if( duc_cmd_prev(con) )
		CMD_FAIL;

	CMD_OK;
}

CMD(cmd_pause)
{
	ARG_NONE;
	if( duc_cmd_pause(con) )
		CMD_FAIL;

	CMD_OK;
}

CMD(cmd_status)
{
	int stat;

	ARG_NONE;
	if( 0 > ( stat = duc_cmd_status(con)))
		CMD_FAIL;

	switch(stat){
		case 0:
			dmsg_msg("stopped\n");
			break;

		case 1:
			dmsg_msg("playing\n");
			break;

		case 2:
			dmsg_msg("paused\n");
			break;

	}

	CMD_OK;
}

// TODO: remove curtrack cmd
CMD(cmd_curtrack)
{
	duc_track *t;
	char buf[BUFLENTRACK];

	ARG_NONE;
	if( NULL == (t = duc_cmd_curtrack(con)))
		CMD_FAIL;

	dmsg_msg( "%s\n\n", dfmt_trackhead(buf, BUFLENTRACK));
	dmsg_msg( "%s\n", dfmt_track(buf, BUFLENTRACK, t));
	duc_track_free(t);
	CMD_OK;
}

CMD(cmd_info)
{
	duc_track *t;
	duc_it_tag *tit;
	duc_it_history *hit;
	char buf[BUFLENTRACK];

	ARG_NONE;

	/* track itself */
	if( NULL == (t = duc_cmd_curtrack(con)))
		CMD_FAIL;

	dmsg_msg( "%s\n\n", dfmt_trackhead(buf, BUFLENTRACK));
	dmsg_msg( "%s\n", dfmt_track(buf, BUFLENTRACK, t));

	//TODO: show info about album

	/* history */
	if( NULL == (hit = duc_cmd_historytrack(con, t->id, 10 ))){
		duc_track_free(t);
		CMD_FAIL;
	}

	dmsg_msg( "\nhistory:\n" );
	dmsg_dump_history(hit);
	duc_it_history_done(hit);

	/* tags */
	if( NULL == (tit = duc_cmd_tracktaglist( con, t->id ))){
		duc_track_free(t);
		CMD_FAIL;
	}

	dmsg_msg( "\ntags:\n" );
	dmsg_dump_tags( tit );
	duc_it_tag_done( tit );


	duc_track_free(t);
	CMD_OK;
}

CMD(cmd_gap)
{
	int gap;

	ARG_NONE;
	if( 0 > (gap = duc_cmd_gap(con)))
		CMD_FAIL;

	dmsg_msg( "delay (gap) before starting next track: %d sec\n", gap );
	CMD_OK;
}

CMD(cmd_gapset)
{
	int gap;
	char *end;

	ARG_NEED("seconds");

	gap = strtol(line, &end, 10 );
	if( *end )
		CMD_FAIL_BADARG("seconds");

	if( duc_cmd_gapset(con, gap ))
		CMD_FAIL;

	dmsg_msg( "delay (gap) set to %d sec\n", gap );
	CMD_OK;
}

CMD(cmd_random)
{
	int random;

	ARG_NONE;
	if( 0 > (random = duc_cmd_random(con)))
		CMD_FAIL;

	dmsg_msg( "random play (when queue is empty) is %s\n", 
			random ? "on" : "off" );
	CMD_OK;
}

CMD(cmd_randomset)
{
	int random = 0;

	ARG_NEED("random_status");

	if( -1 == ( random = atobool( line )))
		CMD_FAIL_BADARG("random_status");

	if( duc_cmd_randomset(con, random))
		CMD_FAIL;

	CMD_OK;
}

/************************************************************
 * commands: track
 */

CMD(cmd_trackcount)
{
	int num;

	ARG_NONE;

	if( 0 > ( num = duc_cmd_tracks(con)))
		CMD_FAIL;

	dmsg_msg( "available tracks: %d\n", num );
	CMD_OK;
}

// TODO: remove track2id cmd
CMD(cmd_track2id)
{
	char *s;
	char *e;
	int albumid;
	int num;
	int id;

	// TODO: deal with special "c" track

	(const char *)s = line;
	albumid = strtol(s, &e, 10 );
	if( s == e )
		CMD_FAIL_BADARG( "albumID" );

	s = e + strspn(e, "\t /");
	num = strtol(s, &e, 10 );
	if( s==e || *e )
		CMD_FAIL_BADARG( "trackNr" );

	if( 0 > (id = duc_cmd_track2id( con, albumid, num))){
		dmsg_msg( "track not found" );
		return -ENOENT;
	}

	dmsg_msg( "track %s has ID: %d\n", dfmt_trackid(albumid, num), id );
	CMD_OK;
}


CMD(cmd_trackget)
{
	int num;
	duc_track *t;
	char buf[BUFLENTRACK];

	ARG_NEED("trackID");

	if( 0 > (num = track2id( con, line, NULL )))
		CMD_FAIL_BADARG( "trackID" );

	if( NULL == ( t = duc_cmd_trackget(con, num )))
		CMD_FAIL;

	dmsg_msg( "%s\n\n", dfmt_trackhead(buf, BUFLENTRACK));
	dmsg_msg( "%s\n", dfmt_track(buf,BUFLENTRACK,t));
	duc_track_free( t );
	CMD_OK;
}

CMD(cmd_tracksearch)
{
	duc_it_track *it;

	ARG_NEED("substr");

	if( NULL == (it = duc_cmd_tracksearch( con, line )))
		CMD_FAIL;

	dmsg_dump_tracks(it);
	duc_it_track_done(it);
	CMD_OK;
}

CMD(cmd_tracksearchf)
{
	duc_it_track *it;

	ARG_NEED("filter");

	if( NULL == (it = duc_cmd_tracksearchf( con, line )))
		CMD_FAIL;

	dmsg_dump_tracks(it);
	duc_it_track_done(it);
	CMD_OK;
}

// TODO: use album name
// TODO: album name completer
CMD(cmd_tracksalbum)
{
	int id;
	duc_it_track *it;

	ARG_NEED("albumID");

	id = atoi(line);
	if( NULL == (it = duc_cmd_tracksalbum( con, id )))
		CMD_FAIL;

	dmsg_dump_tracks(it);
	duc_it_track_done(it);
	CMD_OK;
}

// TODO: use artist name
// TODO: artist name completer
CMD(cmd_tracksartist)
{
	int id;
	duc_it_track *it;

	ARG_NEED("artistID");

	id = atoi(line);
	if( NULL == (it = duc_cmd_tracksartist( con, id )))
		CMD_FAIL;

	dmsg_dump_tracks(it);
	duc_it_track_done(it);
	CMD_OK;
}

// TODO: track_alter


/************************************************************
 * commands: random/filter
 */

CMD(cmd_filter)
{
	char *f;

	ARG_NONE;

	if( NULL == (f = duc_cmd_filter( con )))
		CMD_FAIL;

	dmsg_msg( "current filter: %s\n", f );
	free(f);
	CMD_OK;
}

CMD(cmd_filterset)
{
	if( duc_cmd_filterset(con, line))
		CMD_FAIL;

	CMD_OK;
}

CMD(cmd_filterstat)
{
	int n;

	ARG_NONE;

	if( 0 > ( n = duc_cmd_filterstat(con)))
		CMD_FAIL;

	dmsg_msg( "tracks matching filter: %d\n", n );
	CMD_OK;
}

CMD(cmd_randomtop)
{
	int num = 20;
	char *end;
	duc_it_track *it;

	if( *line ){
		num = strtol(line, &end, 10 );
		if( *end )
			CMD_FAIL_BADARG("maxnumber");
	}

	if( NULL == (it = duc_cmd_randomtop( con, num )))
		CMD_FAIL;

	dmsg_dump_tracks(it);
	duc_it_track_done(it);
	CMD_OK;
}


/************************************************************
 * commands: queue 
 */

CMD(cmd_queuelist)
{
	duc_it_queue *it;

	ARG_NONE;

	if( NULL == (it = duc_cmd_queuelist( con )))
		CMD_FAIL;

	dmsg_dump_queue(it);
	duc_it_queue_done(it);
	CMD_OK;
}

CMD(cmd_queueget)
{
	char buf[BUFLENTRACK];
	int id;
	duc_queue *q;

	ARG_NEED("queueID");
	id = atoi(line);
	if( NULL == (q = duc_cmd_queueget(con, id)))
		CMD_FAIL;

	dmsg_msg("%s\n\n", dfmt_queuehead(buf, BUFLENTRACK));
	dmsg_msg("%s\n", dfmt_queue(buf, BUFLENTRACK,q));
	duc_queue_free(q);
	CMD_OK;
}

// TODO: queue a specified list of tracks
CMD(cmd_queueadd)
{
	int id;

	ARG_NEED("trackID");

	if( 0 > (id = track2id(con, line, NULL)))
		CMD_FAIL_BADARG("trackID");

	if( 0 > duc_cmd_queueadd(con, id))
		CMD_FAIL;

	CMD_OK;
}

// TODO: queue a specified list of albums
// TODO: queue current album
CMD(cmd_queuealbum)
{
	int id;

	ARG_NEED("albumID");

	if( 0 > (id = track2id(con, line, NULL)))
		CMD_FAIL_BADARG("trackID");

	if( 0 > duc_cmd_queuealbum(con, id))
		CMD_FAIL;

	CMD_OK;
}


// TODO: unqueue range, by user, client, album, track
// TODO: unqueue a list
CMD(cmd_queuedel)
{
	int id;

	ARG_NEED("queueID");

	id = atoi(line);
	if( duc_cmd_queuedel(con, id ))
		CMD_FAIL;

	CMD_OK;
}

CMD(cmd_queueclear)
{
	ARG_NONE;

	if( duc_cmd_queueclear( con ))
		CMD_FAIL;

	CMD_OK;
}

/************************************************************
 * commands: sleep
 */

CMD(cmd_sleep)
{
	int r;

	ARG_NONE;

	if( 0 > ( r = duc_cmd_sleep( con )))
		CMD_FAIL;

	dmsg_msg( "falling asleep in %d sek\n", r );
	CMD_OK;
}

CMD(cmd_sleepset)
{
	int t;
	char *end;

	ARG_NEED("seconds");

	t = strtol(line, &end, 10 );
	if( *end )
		CMD_FAIL_BADARG("seconds");

	if( duc_cmd_sleepset(con, t ))
		CMD_FAIL;

	CMD_OK;
}


/************************************************************
 * commands: history
 */

CMD(cmd_history)
{
	int num = 20;
	char *end;
	duc_it_history *it;

	if( *line ){
		num = strtol(line, &end, 10 );
		if( *end )
			CMD_FAIL_BADARG("maxnumber");
	}

	if( NULL == (it = duc_cmd_history(con, num )))
		CMD_FAIL;

	dmsg_dump_history(it);
	duc_it_history_done(it);
	CMD_OK;
}

CMD(cmd_historytrack)
{
	int num = 20;
	int id;
	duc_it_history *it;
	char *end;

	if( 0 > (id = track2id(con, line, &end)))
		CMD_FAIL_BADARG("trackID");

	if( *end ){
		num = strtol(end, &end, 10 );
		if( *end )
			CMD_FAIL_BADARG("maxnumber");
	}

	if( NULL == (it = duc_cmd_historytrack(con, id, num )))
		CMD_FAIL;

	dmsg_dump_history(it);
	duc_it_history_done(it);
	CMD_OK;
}

/************************************************************
 * commands: tag
 */

// TODO: remove tag2id cmd
CMD(cmd_tag2id)
{
	int id;

	ARG_NEED("tagName");
	if( 0 > (id = duc_cmd_tag2id(con, line)))
		CMD_FAIL;

	dmsg_msg( "tag has ID %d\n", id );
	CMD_OK;
}

CMD(cmd_tagget)
{
	int id;
	char *e;
	duc_tag *t;
	char buf[BUFLENTAG];

	ARG_NEED("tagID");

	id = tag2id( con, line, &e );
	if( *e )
		CMD_FAIL_BADARG("tagID");

	if( NULL == (t= duc_cmd_tagget(con, id)))
		CMD_FAIL;

	dmsg_msg( "%s\n\n", dfmt_taghead(buf,BUFLENTAG));
	dmsg_msg( "%s\n", dfmt_tag(buf,BUFLENTAG, t));
	duc_tag_free(t);
	CMD_OK;
}

CMD(cmd_taglist)
{
	duc_it_tag *it;

	ARG_NONE;

	if( NULL == (it = duc_cmd_taglist( con )))
		CMD_FAIL;

	dmsg_dump_tags( it );
	duc_it_tag_done( it );
	CMD_OK;
}

CMD(cmd_tagadd)
{
	int id;

	ARG_NEED( "tagName" );
	if( 0 > (id = duc_cmd_tagadd( con, line )))
		CMD_FAIL;

	dmsg_msg( "added with id %d\n", id );
	CMD_OK;
}

CMD(cmd_tagsetname)
{
	char *e;
	int id;


	ARG_NEED("tagID");
	id = tag2id( con, line, &e );
	if( line == e )
		CMD_FAIL_BADARG("tagID");

	e += strspn(e, "\t ");
	if( duc_cmd_tagsetname(con, id, e))
		CMD_FAIL;

	CMD_OK;
}

CMD(cmd_tagsetdesc)
{
	char *e;
	int id;


	ARG_NEED("tagID");
	id = tag2id( con, line, &e );
	if( line == e )
		CMD_FAIL_BADARG("tagID");

	e += strspn(e, "\t ");
	if( duc_cmd_tagsetdesc(con, id, e))
		CMD_FAIL;

	CMD_OK;
}

CMD(cmd_tagdel)
{
	int id;
	char *e;

	ARG_NEED("tagID");
	id = tag2id( con, line, &e );
	if( *e )
		CMD_FAIL_BADARG("tagID");

	if( duc_cmd_tagdel(con, id))
		CMD_FAIL;

	CMD_OK;
}

CMD(cmd_tracktaglist)
{
	int id;
	char *e;
	duc_it_tag *it;

	ARG_NEED("trackID");
	id = track2id( con, line, &e );
	if( *e )
		CMD_FAIL_BADARG("trackID");

	if( NULL == (it = duc_cmd_tracktaglist( con, id )))
		CMD_FAIL;

	dmsg_dump_tags( it );
	duc_it_tag_done( it );
	CMD_OK;
}

CMD(cmd_tracktagged)
{
	int tid, trid;
	char *s, *e;
	int r;

	ARG_NEED("trackID");
	trid = track2id( con, line, &e );
	if( e == line )
		CMD_FAIL_BADARG("trackID");

	s = e += strspn( e, "\t " );
	tid = tag2id(con, s, &e );
	if( *e )
		CMD_FAIL_BADARG("tagID");

	if( 0 > (r = duc_cmd_tracktagged( con, trid, tid )))
		CMD_FAIL;

	dmsg_msg( "tag is%s set for this track\n", r ? "": " not" );
	CMD_OK;
}

CMD(cmd_tracktagset)
{
	int tid, trid;
	char *s, *e;

	ARG_NEED("trackID");
	trid = track2id( con, line, &e );
	if( e == line )
		CMD_FAIL_BADARG("trackID");

	s = e += strspn( e, "\t " );
	tid = tag2id(con, s, &e );
	if( *e )
		CMD_FAIL_BADARG("tagID");

	if( duc_cmd_tracktagset( con, trid, tid ))
		CMD_FAIL;

	dmsg_msg( "set tag\n");
	CMD_OK;
}

CMD(cmd_tracktagdel)
{
	int tid, trid;
	char *s, *e;

	ARG_NEED("trackID");
	trid = track2id( con, line, &e );
	if( e == line )
		CMD_FAIL_BADARG("trackID");

	s = e += strspn( e, "\t " );
	tid = tag2id(con, s, &e );
	if( *e )
		CMD_FAIL_BADARG("tagID");

	if( duc_cmd_tracktagdel( con, trid, tid ))
		CMD_FAIL;

	dmsg_msg( "tag deleted\n");
	CMD_OK;
}

//TODO: album commands
//TODO: artist commands

/************************************************************
 * command list
 */


typedef struct _t_command {
	char *name;
	duc_cgen gen;
	t_action action;
} t_command;

static t_command commands[] = {
	{ "help", NULL, cmd_help },
	{ "raw", cgen_raw, cmd_raw },
	{ "open", NULL, cmd_open },

	{ "clientlist", NULL, cmd_clientlist },
	{ "clientclose", NULL, cmd_clientclose },
	{ "clientcloseuser", NULL, cmd_clientcloseuser },

	{ "user2id", NULL, cmd_user2id },
	{ "userget", NULL, cmd_userget },
	{ "userlist", NULL, cmd_userlist },
	{ "usersetpass", NULL, cmd_usersetpass },
	{ "usersetright", NULL, cmd_usersetright },
	{ "useradd", NULL, cmd_useradd },
	{ "userdel", NULL, cmd_userdel },

	{ "play", NULL, cmd_play },
	{ "stop", NULL, cmd_stop },
	{ "next", NULL, cmd_next },
	{ "prev", NULL, cmd_prev },
	{ "pause", NULL, cmd_pause },
	{ "status", NULL, cmd_status },
	{ "curtrack", NULL, cmd_curtrack },
	{ "info", NULL, cmd_info },
	{ "gap", NULL, cmd_gap },
	{ "gapset", NULL, cmd_gapset },
	{ "random", NULL, cmd_random },
	{ "randomset", NULL, cmd_randomset },

	{ "trackcount", NULL, cmd_trackcount },
	{ "track2id", NULL, cmd_track2id },
	{ "trackget", NULL, cmd_trackget },
	{ "tracksearch", NULL, cmd_tracksearch },
	{ "tracksearchf", NULL, cmd_tracksearchf },
	{ "tracksalbum", NULL, cmd_tracksalbum },
	{ "tracksartist", NULL, cmd_tracksartist },

	{ "filter", NULL, cmd_filter },
	{ "filterset", NULL, cmd_filterset },
	{ "filterstat", NULL, cmd_filterstat },
	{ "randomtop", NULL, cmd_randomtop },

	{ "queueget", NULL, cmd_queueget },
	{ "queuelist", NULL, cmd_queuelist },
	{ "queueadd", NULL, cmd_queueadd },
	{ "queuealbum", NULL, cmd_queuealbum },
	{ "queuedel", NULL, cmd_queuedel },
	{ "queueclear", NULL, cmd_queueclear },

	{ "sleep", NULL, cmd_sleep },
	{ "sleepset", NULL, cmd_sleepset },

	{ "history", NULL, cmd_history },
	{ "historytrack", NULL, cmd_historytrack },

	{ "tag2id", NULL, cmd_tag2id },
	{ "tagget", NULL, cmd_tagget },
	{ "taglist", NULL, cmd_taglist },
	{ "tagadd", NULL, cmd_tagadd },
	{ "tagsetname", NULL, cmd_tagsetname },
	{ "tagsetdesc", NULL, cmd_tagsetdesc },
	{ "tagdel", NULL, cmd_tagdel },

	{ "tracktaglist", NULL, cmd_tracktaglist },
	{ "tracktagged", NULL, cmd_tracktagged },
	{ "tracktagset", NULL, cmd_tracktagset },
	{ "tracktagdel", NULL, cmd_tracktagdel },

	{ NULL, NULL, NULL }
};


static t_command *command_nfind( const char *cmd, unsigned int len )
{
	int c;

	for( c = 0; commands[c].name; ++c ){
		if( len != strlen(commands[c].name))
			continue;

		if( 0 == strncasecmp(cmd, commands[c].name, len )){
			return &commands[c];
		}
	}

	return NULL;
}

#define WSPACE	"\t "
int duc_cmd( dudlc *con, const char *line )
{
	int len;
	int skip;
	t_command *cmd;

	/* skip leading whitespace */
	line += strspn( line, WSPACE );

	len = strcspn( line, WSPACE );
	if( NULL != (cmd = command_nfind( line, len ))){
		skip = strspn( line + len, WSPACE );
		if( cmd->action )
			return cmd->action( con, line + len + skip );

	}

	dmsg_msg( "no such command - try to send it with 'raw'?\n" );
	return -1;
}

/* toplevel completer */
CGEN(cgen_top)
{
	static int c;
	static unsigned int len;
	char *name;

	(void) con;
	/* TODO use "help" to find currently allowed commands for cgen_top? */
	if( state == 0 ){
		c = 0;
		len = strlen(text);
	}

	while( NULL != (name = commands[c].name) ){
		c++;
		
		if( strncasecmp(text,name,len) == 0 )
			return strdup(name);
	}

	return NULL;
}

duc_cgen duc_cgen_find( dudlc *con, const char *line, unsigned int pos )
{
	int len;
	t_command *cmd;

	(void)con;

	/* skip leading whitespace */
	len = strspn( line, WSPACE );
	line += len;
	pos -= len;

	/* begin of line -> complete the command name */
	if( pos == 0 )
		return cgen_top;

	/* else try to complete command arguments */
	// TODO: make completer finding more elegant
	len = strcspn( line, WSPACE );
	if( NULL != (cmd = command_nfind( line, len )))
		return cmd->gen;

	return NULL;
}


