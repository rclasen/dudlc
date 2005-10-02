#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>

#include <dudlc.h>
#include <dudlc/proto.h>

#include "dudlccmd.h"
#include "parseval.h"
#include "idlist.h"

#define SKIPSPACE(str)	while(isspace(*str)){str++;};

typedef int (*t_idl_itfunc)( dudlc *dudl, int id, void *data );

static int idl_iterate( t_idlist *idl, t_idl_itfunc func, dudlc *dudl, void *data )
{
	int id;

	for( id = idl_begin(idl); id >= 0; id = idl_next(idl)){
		if( -1 == (*func)(dudl, id, data )){
			if( 0 > duc_fd(dudl) ){
				return -1;
			}
		}
	}

	return 0;
}


/************************************************************
 *
 */

#define CMD(func)	static int func( dudlc *dudl, void **argv )
#define GENC(func)	static char *func( dudlc *dudl, const char *text, int state )

CMD(c_help);
CMD(c_helpcmd);
GENC(lst_cmd);

/************************************************************
 *
 */

typedef void *(*t_arg_parse)( dudlc *dudl, char *in, char **end );
typedef void (*t_arg_free)( void *data );

typedef struct _t_cmd_arg {
	char *name;
	int opt;
	t_arg_parse parse;
	duc_cgen genc;
	t_arg_free free;
} t_cmd_arg;

#define AP(func)	(t_arg_parse)func
#define AF(func)	(t_arg_free)func

/************************************************************
 *
 */

typedef int(*t_cmd_run)(dudlc *dudl, void **argv );

typedef struct _t_cmd {
	char *name;
	t_cmd_run run;
	t_cmd_arg *args;
	char *help;
} t_cmd;

/************************************************************
 *
 */

CMD(c_open)
{
	(void)argv;
	return duc_open(dudl);
}

static char **get_commands( dudlc *dudl, const char *text )
{
	duc_it_help *it;
	char *s;
	char **helpv = NULL;
	int helpc = 0;
	int len;
	
	len = strlen(text);

	it = duc_cmd_help(dudl);
	for( s = duc_it_help_cur(it); s; s = duc_it_help_next(it) ){
		char **tmp;

		if( 0 != strncasecmp( s, text, len) ){
			free(s);
			continue;
		}

		tmp = realloc( helpv, sizeof(char*) * (helpc +2) );
		if( tmp == NULL ){
			free(s);
			break;
		}

		helpv = tmp;
		helpv[helpc++] = s;
		helpv[helpc] = NULL;
	}
	duc_it_help_done(it);

	return helpv;
}

GENC(lst_rcmd)
{
	static char **helpv = NULL;
	static char **hnext = NULL;

	if( state == 0 ){
		char **h;

		for( h = helpv; h && *h; ++h )
			free(*h);
		free(helpv);

		hnext = helpv = get_commands( dudl, text );
	}

	if( hnext && *hnext )
		return strdup(*hnext++);

	return NULL;
}

CMD(c_raw)
{
	char *line = (char*)argv[0];
	int r;

	_duc_send(dudl, line );
	while( ! (r = _duc_rnext(dudl) )){
		dmsg_msg( "%s\n", _duc_rline(dudl) );
	}

	if( r == -1 )
		return -1;

	dmsg_msg( "return code: %s\n", _duc_rcode(dudl));
	return 0;
}

/************************************************************
 *
 */

CMD(c_clientlist)
{
	duc_it_client *it;

	(void)argv;
	if( NULL == (it = duc_cmd_clientlist(dudl)))
		return -1;

	dmsg_dump_clients(it);
	duc_it_client_done(it);

	return 0;
}

static int i_clientclose( dudlc *dudl, int id, void *data )
{
	(void)data;

	return duc_cmd_clientclose( dudl, id );
}

CMD(c_clientclose)
{

	return idl_iterate((t_idlist *)argv[0], i_clientclose, dudl, NULL );
}

static int i_clientcloseuser( dudlc *dudl, int id, void *data )
{
	(void)data;

	return duc_cmd_clientcloseuser( dudl, id );
}

CMD(c_clientcloseuser)
{
	return idl_iterate((t_idlist *)argv[0], i_clientcloseuser, dudl, NULL );
}

/************************************************************
 *
 */

CMD(c_userget)
{
	int uid = (int)argv[0];
	duc_user *u;

	if( NULL == (u = duc_cmd_userget(dudl, uid )))
		return 1;

	dmsg_dump_user(u);
	duc_user_free(u);

	return 0;
}

CMD(c_userlist)
{
	duc_it_user *it;

	(void)argv;
	if( NULL == (it = duc_cmd_userlist(dudl)))
		return -1;

	dmsg_dump_users(it);
	duc_it_user_done(it);

	return 0;
}

CMD(c_usersetpass)
{
	int uid = (int)argv[0];
	char *pwd = (char*)argv[1];

	return duc_cmd_usersetpass(dudl, uid, pwd );
}

CMD(c_usersetright)
{
	int uid = (int)argv[0];
	int right = (int)argv[1];

	return duc_cmd_usersetright(dudl, uid, right );
}

CMD(c_useradd)
{
	char *usr = (char*)argv[0];

	if( 0 > duc_cmd_useradd(dudl, usr ))
		return -1;
	return 0;
}

CMD(c_userdel)
{
	int uid = (int)argv[0];

	return duc_cmd_userdel(dudl, uid );
}


/************************************************************
 *
 */

CMD(c_play)
{
	(void)argv;
	return duc_cmd_play(dudl);
}

CMD(c_stop)
{
	(void)argv;
	return duc_cmd_stop(dudl);;
}

CMD(c_next)
{
	(void)argv;
	return duc_cmd_next(dudl);;
}

CMD(c_prev)
{
	(void)argv;
	return duc_cmd_prev(dudl);;
}

CMD(c_pause)
{
	(void)argv;
	return duc_cmd_pause(dudl);
}

CMD(c_status)
{
	int stat;

	(void)argv;
	switch( stat = duc_cmd_status(dudl)){
		case 0:
			dmsg_msg("stopped\n");
			break;
		case 1:
			dmsg_msg("playing\n");
			break;
		case 2:
			dmsg_msg("paused\n");
			break;

		default:
			return -1;
	}
	return 0;
}

CMD(c_info)
{
	duc_track *t;
	duc_it_tag *tit;
	duc_it_history *hit;

	(void)argv;
	if( NULL == (t = duc_cmd_curtrack(dudl)))
		return -1;
	dmsg_msg("current track:\n" );
	dmsg_dump_track(t);

	if( NULL == (hit = duc_cmd_historytrack( dudl, t->id, 10))){
		duc_track_free(t);
		return -1;
	}
	dmsg_msg("\nhistory:\n");
	dmsg_dump_historys(hit);
	duc_it_history_done(hit);

	if( NULL == (tit = duc_cmd_tracktaglist( dudl, t->id ))){
		duc_track_free(t);
		return -1;
	}
	dmsg_msg("\ntags:\n");
	dmsg_dump_tags(tit);
	duc_it_tag_done(tit);

	duc_track_free(t);
	return 0;
}

CMD(c_gap)
{
	int gap;

	(void)argv;
	if( 0 > (gap = duc_cmd_gap(dudl)))
		return -1;

	dmsg_msg( "gap between tracks: %s sec\n", gap );
	return 0;
}

CMD(c_gapset)
{
	int delay = (int)argv[0];

	return duc_cmd_gapset(dudl, delay);
}

CMD(c_random)
{
	int rnd;

	(void)argv;
	if( 0 > (rnd = duc_cmd_random(dudl)))
		return -1;

	dmsg_msg( "random play (when queue is empty) is %s\n", 
			rnd ? "on" : "off" );
	return 0;
}

CMD(c_randomset)
{
	int rnd = (int)argv[0];
	return duc_cmd_randomset(dudl, rnd);
}

/************************************************************
 *
 */

CMD(c_trackcount)
{
	int cnt;

	(void)argv;
	if( 0 > (cnt = duc_cmd_tracks(dudl)))
		return -1;

	dmsg_msg( "available tracks: %d\n", cnt );
	return 0;
}

static int i_trackget( dudlc *dudl, int id, void *data )
{
	char buf[BUFLENTRACK];
	duc_track *t;

	(void)data;

	if( NULL == ( t = duc_cmd_trackget( dudl, id ) ))
		return -1;

	dmsg_msg("%s\n", dfmt_track(buf, BUFLENTRACK, t));

	duc_track_free(t);
	return 0;
}

CMD(c_trackget)
{
	char buf[BUFLENTRACK];

	dmsg_msg("%s\n\n", dfmt_trackhead(buf, BUFLENTRACK));
	return idl_iterate((t_idlist *)argv[0], i_trackget, dudl, NULL );
}

CMD(c_tracksearchf)
{
	char *filt = (char*)argv[0];
	duc_it_track *it;

	if( NULL == (it = duc_cmd_tracksearchf( dudl, filt )))
		return -1;

	dmsg_dump_tracks(it);
	duc_it_track_done(it);

	return 0;
}

/************************************************************
 *
 */

CMD(c_filter)
{
	char *flt;

	(void)argv;
	if( NULL == (flt = duc_cmd_filter(dudl)))
		return -1;

	dmsg_msg( "current filter: %s\n", flt);
	free(flt);
	return 0;
}

CMD(c_filterset)
{
	char *filt = (char*)argv[0];
	return duc_cmd_filterset(dudl, filt );
}

CMD(c_filterstat)
{
	int cnt;

	(void)argv;
	if( 0 > (cnt = duc_cmd_filterstat(dudl)))
		return -1;

	dmsg_msg( "tracks matching filter: %s\n", cnt );
	return 0;
}

CMD(c_filtertop)
{
	int num = (int)argv[0];
	duc_it_track *it;

	if( NULL == (it = duc_cmd_randomtop( dudl, num )))
		return -1;

	dmsg_dump_tracks(it);
	duc_it_track_done(it);

	return 0;
}

/************************************************************
 *
 */

CMD(c_queuelist)
{
	duc_it_queue *it;

	(void)argv;
	if( NULL == (it = duc_cmd_queuelist( dudl )))
		return -1;

	dmsg_dump_queues(it);
	duc_it_queue_done(it);

	return 0;
}

static int i_queueget( dudlc *dudl, int id, void *data )
{
	char buf[BUFLENQUEUE];
	duc_queue *t;

	(void)data;

	if( NULL == ( t = duc_cmd_queueget( dudl, id ) ))
		return -1;

	dmsg_msg("%s\n", dfmt_queue(buf, BUFLENQUEUE, t));

	duc_queue_free(t);
	return 0;
}

CMD(c_queueget)
{
	char buf[BUFLENQUEUE];

	dmsg_msg("%s\n\n", dfmt_queuehead(buf, BUFLENQUEUE));
	return idl_iterate((t_idlist *)argv[0], i_queueget, dudl, NULL );
}

static int i_queueadd( dudlc *dudl, int id, void *data )
{
	(void)data;

	if( -1 == duc_cmd_queueadd( dudl, id ) )
		return -1;

	return 0;
}

CMD(c_queueadd)
{
	return idl_iterate((t_idlist *)argv[0], i_queueadd, dudl, NULL );
}

static int i_queuedel( dudlc *dudl, int id, void *data )
{
	(void)data;

	return duc_cmd_queuedel( dudl, id );
}

CMD(c_queuedel)
{
	return idl_iterate((t_idlist *)argv[0], i_queuedel, dudl, NULL );
}

CMD(c_queueclear)
{
	(void)argv;
	return duc_cmd_queueclear( dudl );
}

/************************************************************
 *
 */

CMD(c_sleep)
{
	int cnt;

	(void)argv;
	if( 0 > (cnt = duc_cmd_sleep(dudl)))
		return -1;

	dmsg_msg( "falling asleep in %d sec\n", cnt );
	return 0;
}

CMD(c_sleepset)
{
	int delay = (int)argv[0];
	return duc_cmd_sleepset(dudl, delay);
}

/************************************************************
 *
 */

CMD(c_history)
{
	int num = (int)argv[0];
	duc_it_history *it;

	if( NULL == (it = duc_cmd_history( dudl, num )))
		return -1;

	dmsg_dump_historys(it);
	duc_it_history_done(it);

	return 0;
}

CMD(c_historytrack)
{
	int id = (int)argv[0];
	int num = (int)argv[1];
	duc_it_history *hit;

	if( NULL == (hit = duc_cmd_historytrack( dudl, id, num )))
		return -1;
	dmsg_dump_historys(hit);
	duc_it_history_done(hit);
	return 0;
}

/************************************************************
 *
 */

CMD(c_taglist)
{
	duc_it_tag *it;

	(void)argv;
	if( NULL == (it = duc_cmd_taglist( dudl )))
		return -1;

	dmsg_dump_tags(it);
	duc_it_tag_done(it);

	return 0;
}

static int i_tagget( dudlc *dudl, int id, void *data )
{
	char buf[BUFLENTAG];
	duc_tag *t;

	(void)data;

	if( NULL == ( t = duc_cmd_tagget( dudl, id ) ))
		return -1;

	dmsg_msg("%s\n", dfmt_tag(buf, BUFLENTAG, t));

	duc_tag_free(t);
	return 0;
}

CMD(c_tagget)
{
	char buf[BUFLENTAG];

	dmsg_msg("%s\n\n", dfmt_taghead(buf, BUFLENQUEUE));
	return idl_iterate((t_idlist *)argv[0], i_tagget, dudl, NULL );
}

CMD(c_tagadd)
{
	char *name = (char*)argv[0];

	if( -1 == duc_cmd_tagadd(dudl, name))
		return -1;

	return 0;
}

CMD(c_tagsetname)
{
	int id = (int)argv[0];
	char *name = (char*)argv[1];

	return duc_cmd_tagsetname(dudl, id, name);
}

CMD(c_tagsetdesc)
{
	int id = (int)argv[0];
	char *desc = (char*)argv[1];

	return duc_cmd_tagsetdesc(dudl, id, desc );
}

static int i_tagdel( dudlc *dudl, int id, void *data )
{
	(void)data;

	return duc_cmd_tagdel( dudl, id );
}

CMD(c_tagdel)
{
	return idl_iterate((t_idlist *)argv[0], i_tagdel, dudl, NULL );
}

/************************************************************
 *
 */

static int i_tracktaglist( dudlc *dudl, int id, void *data )
{
	duc_track *tr;
	duc_it_tag *it;
	duc_tag *ta;
	char buf[4096] = "";

	(void)data;
	if( NULL == (tr = duc_cmd_trackget(dudl, id )))
		return -1;
	strcat( buf, dfmt_trackid( tr->album->id, tr->albumnr));
	strcat( buf, ":" );
	duc_track_free(tr);

	if( NULL == (it = duc_cmd_tracktaglist( dudl, id )))
		return -1;

	for( ta = duc_it_tag_cur(it); ta; ta = duc_it_tag_next(it)){
		strcat( buf, " " ); // TODO: use , as seperator (cut'n'paste)
		strcat( buf, ta->name );
	}
	duc_it_tag_done(it);

	dmsg_msg("%s\n",buf);

	return 0;
}

CMD(c_tracktaglist)
{
	return idl_iterate((t_idlist *)argv[0], i_tracktaglist, dudl, NULL );
}

static int i_tracktagsettrack( dudlc *dudl, int id, void *data )
{
	int trackid = (int)data;
	return duc_cmd_tracktagset(dudl, trackid, id);
}

static int i_tracktagset( dudlc *dudl, int id, void *data )
{
	return idl_iterate((t_idlist *)data, i_tracktagsettrack, dudl, (void*)id );
}

CMD(c_tracktagset)
{
	t_idlist *tracks = (t_idlist*)argv[0];
	t_idlist *tags = (t_idlist*)argv[1];
	return idl_iterate(tracks, i_tracktagset, dudl, tags );
}

static int i_tracktagdeltrack( dudlc *dudl, int id, void *data )
{
	int trackid = (int)data;
	return duc_cmd_tracktagdel(dudl, trackid, id);
}

static int i_tracktagdel( dudlc *dudl, int id, void *data )
{
	return idl_iterate((t_idlist *)data, i_tracktagdeltrack, dudl, (void*)id );
}

CMD(c_tracktagdel)
{
	t_idlist *tracks = (t_idlist*)argv[0];
	t_idlist *tags = (t_idlist*)argv[1];
	return idl_iterate(tracks, i_tracktagdel, dudl, tags );
}

/************************************************************
 *
 */

CMD(c_albumlist)
{
	duc_it_album *it;

	(void)argv;
	if( NULL == (it = duc_cmd_albumlist( dudl )))
		return -1;

	dmsg_dump_albums(it);
	duc_it_album_done(it);

	return 0;
}

static int i_albumget( dudlc *dudl, int id, void *data )
{
	char buf[BUFLENALBUM];
	duc_album *t;

	(void)data;

	if( NULL == ( t = duc_cmd_albumget( dudl, id ) ))
		return -1;

	dmsg_msg("%s\n", dfmt_album(buf, BUFLENALBUM, t));

	duc_album_free(t);
	return 0;
}

CMD(c_albumget)
{
	char buf[BUFLENALBUM];

	dmsg_msg("%s\n\n", dfmt_albumhead(buf, BUFLENQUEUE));
	return idl_iterate((t_idlist *)argv[0], i_albumget, dudl, NULL );
}

CMD(c_albumsearch)
{
	char *fnd = (char*)argv[0];
	duc_it_album *it;

	if( NULL == (it = duc_cmd_albumsearch( dudl, fnd )))
		return -1;

	dmsg_dump_albums(it);
	duc_it_album_done(it);

	return 0;
}

CMD(c_albumsartist)
{
	int id = (int)argv[0];
	duc_it_album *it;

	if( NULL == (it = duc_cmd_albumsartist( dudl, id )))
		return -1;

	dmsg_dump_albums(it);
	duc_it_album_done(it);

	return 0;
}

/************************************************************
 *
 */

CMD(c_artistlist)
{
	duc_it_artist *it;

	(void)argv;
	if( NULL == (it = duc_cmd_artistlist( dudl )))
		return -1;

	dmsg_dump_artists(it);
	duc_it_artist_done(it);

	return 0;
}

static int i_artistget( dudlc *dudl, int id, void *data )
{
	char buf[BUFLENARTIST];
	duc_artist *t;

	(void)data;

	if( NULL == ( t = duc_cmd_artistget( dudl, id ) ))
		return -1;

	dmsg_msg("%s\n", dfmt_artist(buf, BUFLENARTIST, t));

	duc_artist_free(t);
	return 0;
}

CMD(c_artistget)
{
	char buf[BUFLENARTIST];

	dmsg_msg("%s\n\n", dfmt_artisthead(buf, BUFLENQUEUE));
	return idl_iterate((t_idlist *)argv[0], i_artistget, dudl, NULL );
}

CMD(c_artistsearch)
{
	char *fnd = (char*)argv[0];
	duc_it_artist *it;

	if( NULL == (it = duc_cmd_artistsearch( dudl, fnd )))
		return -1;

	dmsg_dump_artists(it);
	duc_it_artist_done(it);

	return 0;
}

CMD(c_artistsetname)
{
	int id = (int)argv[0];
	char *name = (char*)argv[1];
	return duc_cmd_artistsetname( dudl, id, name );
}

CMD(c_artistmerge)
{
	int from = (int)argv[0];
	int to = (int)argv[1];
	return duc_cmd_artistmerge( dudl, from, to );
}

/************************************************************
 *
 */

CMD(c_sfilterlist)
{
	duc_it_sfilter *it;

	(void)argv;
	if( NULL == (it = duc_cmd_sfilterlist( dudl )))
		return -1;

	dmsg_dump_sfilters(it);
	duc_it_sfilter_done(it);

	return 0;
}

static int i_sfilterget( dudlc *dudl, int id, void *data )
{
	char buf[BUFLENSFILTER];
	duc_sfilter *t;

	(void)data;

	if( NULL == ( t = duc_cmd_sfilterget( dudl, id ) ))
		return -1;

	dmsg_msg("%s\n", dfmt_sfilter(buf, BUFLENSFILTER, t));

	duc_sfilter_free(t);
	return 0;
}

CMD(c_sfilterget)
{
	char buf[BUFLENSFILTER];

	dmsg_msg("%s\n\n", dfmt_sfilterhead(buf, BUFLENQUEUE));
	return idl_iterate((t_idlist *)argv[0], i_sfilterget, dudl, NULL );
}

CMD(c_sfilteradd)
{
	char *name = (char*)argv[0];

	if( -1 == duc_cmd_sfilteradd(dudl, name))
		return -1;

	return 0;
}

CMD(c_sfiltersetname)
{
	int id = (int)argv[0];
	char *name = (char*)argv[1];
	return duc_cmd_sfiltersetname(dudl, id,  name);
}

CMD(c_sfiltersetfilter)
{
	int id = (int)argv[0];
	char *flt = (char*)argv[1];
	return duc_cmd_sfiltersetfilter(dudl, id, flt );
}

CMD(c_sfilterload)
{
	int id = (int)argv[0];
	duc_sfilter *filt;

	if( NULL == (filt = duc_cmd_sfilterget(dudl, id)))
		return -1;

	if( -1 == duc_cmd_filterset(dudl, filt->filter)){
		duc_sfilter_free(filt);
		return -1;
	}

	duc_sfilter_free(filt);
	return 0;
}

CMD(c_sfiltersave)
{
	int id = (int)argv[0];
	char *flt;

	if( NULL == (flt = duc_cmd_filter(dudl)))
		return -1;

	if( -1 == duc_cmd_sfiltersetfilter(dudl, id, flt)){
		free(flt);
		return -1;
	}

	free(flt);
	return 0;
}


/************************************************************
 *
 */

#define arg_end	\
	{ NULL,		0, NULL,		NULL,		NULL }
#define arg_rcmd	\
	{ "rcmd",	0, AP(val_string),	lst_rcmd,	AF(free) }
#define arg_name	\
	{ "name",	0, AP(val_name),	NULL,		AF(free) }
#define arg_cmd	\
	{ "cmd",	0, AP(val_name),	lst_cmd,	AF(free) }
#define arg_string	\
	{ "string",	0, AP(val_string),	NULL,		AF(free) }
#define arg_idlist	\
	{ "idlist",	0, AP(val_idlist),	NULL,		AF(idl_done) }
// TODO: lst_user
#define arg_user	\
	{ "user",	0, AP(val_user),	NULL,		NULL }
#define arg_userlist	\
	{ "userlist",	0, AP(val_userlist),	NULL,		AF(idl_done) }
// TODO: lst_right
#define arg_right	\
	{ "right",	0, AP(val_right),	NULL,		NULL }
#define arg_sec	\
	{ "sec",	0, AP(val_uint),	NULL,		NULL }
// TODO: lst_bool
#define arg_bool	\
	{ "bool",	0, AP(val_bool),	NULL,		NULL }
#define arg_track	\
	{ "track",	0, AP(val_track),	NULL,		NULL }
#define arg_tracklist	\
	{ "tracklist",	0, AP(val_tracklist),	NULL,		AF(idl_done) }
// TODO:* lst_taglist
#define arg_taglist	\
	{ "taglist",	0, AP(val_taglist),	NULL,		AF(idl_done) }
// TODO: lst_tag
#define arg_tag	\
	{ "tag",	0, AP(val_tag),		NULL,		NULL }
#define arg_onum	\
	{ "num",	1, AP(val_uint),	NULL,		NULL }
// TODO: lst_artist
#define arg_artist	\
	{ "artist",	0, AP(val_uint),	NULL,		NULL }
#define arg_sfilter	\
	{ "sfilter",	0, AP(val_sfilter),	NULL,		NULL }

t_cmd_arg args_none[]		= { arg_end };
t_cmd_arg args_idlist[]		= { arg_idlist, arg_end };
t_cmd_arg args_userlist[]	= { arg_userlist, arg_end };
t_cmd_arg args_name[]		= { arg_name, arg_end };
t_cmd_arg args_cmd[]		= { arg_cmd, arg_end };
t_cmd_arg args_sec[]		= { arg_sec, arg_end };
t_cmd_arg args_bool[]		= { arg_bool, arg_end };
t_cmd_arg args_track[]		= { arg_track, arg_end };
t_cmd_arg args_tracklist[]	= { arg_tracklist, arg_end };
t_cmd_arg args_string[]		= { arg_string, arg_end };
t_cmd_arg args_onum[]		= { arg_onum, arg_end };
t_cmd_arg args_taglist[]	= { arg_taglist, arg_end };
t_cmd_arg args_sfilter[]	= { arg_sfilter, arg_end };
t_cmd_arg args_artist[]		= { arg_artist, arg_end };
t_cmd_arg args_raw[]		= { arg_rcmd, arg_end };
t_cmd_arg args_uidpass[]	= { arg_user, arg_string, arg_end };
t_cmd_arg args_uidright[]	= { arg_user, arg_right, arg_end };
t_cmd_arg args_tracknum[]	= { arg_track, arg_onum, arg_end };
t_cmd_arg args_tagname[]	= { arg_tag, arg_name, arg_end };
t_cmd_arg args_tagdesc[]	= { arg_tag, arg_string, arg_end };
t_cmd_arg args_tracktaglist[]	= { arg_tracklist, arg_taglist, arg_end };
t_cmd_arg args_artistname[]	= { arg_artist, arg_name, arg_end };
t_cmd_arg args_artistmerge[]	= { arg_artist, arg_artist, arg_end };
t_cmd_arg args_sfiltername[]	= { arg_sfilter, arg_name, arg_end };
t_cmd_arg args_sfilterfilter[]	= { arg_sfilter, arg_string, arg_end };

t_cmd cmds_top[] = {
	{ "open",		c_open,			args_none,
		"open connection to server" },
	{ "help",		c_help,			args_none,
		"show list of commands" },
	{ "helpcmd",		c_helpcmd,		args_cmd,
		"show help for one command" },
	{ "raw",		c_raw,			args_raw,
		"send raw commando to server" },

	{ "clientlist",		c_clientlist,		args_none,
		"list all connected clients" },
	{ "clientclose",	c_clientclose,		args_idlist,
		"disconnect clients" },
	{ "clientcloseuser",	c_clientcloseuser,	args_userlist,
		"disconnect clients owned by certain users" },

	{ "userget",		c_userget,		args_userlist,
		"show user details" },
	{ "userlist",		c_userlist,		args_none,
		"list all users" },
	{ "usersetpass",	c_usersetpass,		args_uidpass,
		"set user's password" },
	{ "usersetright",	c_usersetright,		args_uidright,
		"set user's permissions" },
	{ "useradd",		c_useradd,		args_name,
		"add user" },
	{ "userdel",		c_userdel,		args_userlist,
		"delete user" },

	{ "play",		c_play,			args_none,
		"start playing" },
	{ "stop",		c_stop,			args_none,
		"stop playing" },
	{ "next",		c_next,			args_none,
		"play next track" },
	{ "prev",		c_prev,			args_none,
		"pley previous track" },
	{ "pause",		c_pause,		args_none,
		"pause playing" },
	{ "status",		c_status,		args_none,
		"show current play-status" },
	{ "info",		c_info,			args_none,
		"show more detailed infoe abour currently playing track" },
	{ "gap",		c_gap,		args_none,
		"show the delay between tracks (sec)" },
	{ "gapset",		c_gapset,		args_sec,
		"set the delay between tracks (sec)" },
	{ "random",		c_random,		args_none,
		"is random playback on? (when the queue ist empty)" },
	{ "randomset",		c_randomset,		args_bool,
		"set random playback status" },

	{ "trackcount",		c_trackcount,		args_none,
		"show number of available tracks" },
	{ "trackget",		c_trackget,		args_tracklist,
		"show track details" },
	{ "tracksearchf",	c_tracksearchf,		args_string,
		"search for titles matching filter" },
	// TODO: trackalter

	{ "filter",		c_filter,		args_none,
		"show filter for random that's in use" },
	{ "filterset",		c_filterset,		args_string,
		"set filter for random filter" },
	{ "filterstat",		c_filterstat,		args_none,
		"show filter statistics" },
	{ "filtertop",		c_filtertop,		args_onum,
		"show tracks with highest probability" },

	{ "queuelist",		c_queuelist,		args_none,
		"show all queued titles" },
	{ "queueget",		c_queueget,		args_idlist,
		"show queue details" },
	{ "queueadd",		c_queueadd,		args_tracklist,
		"add titles to queue" },
	{ "queuedel",		c_queuedel,		args_idlist,
		"remove titles from queue" },
	/*
	 * TODO: filter for queue ops
	{ "queuesearchf",	c_queuesearchf,		args_string,
		"show queue details" },
	{ "queueaddf",		c_queueaddf,		args_string,
		"add titles to queue" },
	 * TODO: unqueue by user, client, album, track, filter
	{ "queuedelf",		c_queuedelf,		args_string,
		"remove titles from queue" },
	*/
	{ "queueclear",		c_queueclear,		args_none,
		"remove all titles from queue" },

	{ "sleep",		c_sleep,		args_none,
		"get remaining time until scheduled pause" },
	{ "sleepset",		c_sleepset,		args_sec,
		"schedule pause after specified time" },

	{ "history",		c_history,		args_onum,
		"show last played titles" },
	{ "historytrack",	c_historytrack,		args_tracknum,
		"history for titles" },
	
	{ "taglist",		c_taglist,		args_none,
		"show all tags" },
	{ "tagget",		c_tagget,		args_taglist,
		"show tag details" },
	{ "tagadd",		c_tagadd,		args_name,
		"add tag" },
	{ "tagsetname",		c_tagsetname,		args_tagname,
		"change tag name" },
	{ "tagsetdesc",		c_tagsetdesc,		args_tagdesc,
		"change tag description" },
	{ "tagdel",		c_tagdel,		args_taglist,
		"delete tag" },

	{ "tracktaglist",	c_tracktaglist,		args_tracklist,
		"show tags set for titles" },
	{ "tracktagset",	c_tracktagset,		args_tracktaglist,
		"add tags to titles" },
	{ "tracktagdel",	c_tracktagdel,		args_tracktaglist,
		"remove tags from titles" },

	{ "albumlist",		c_albumlist,		args_none,
		"show all albums" },
	{ "albumget",		c_albumget,		args_idlist,
		"show album details" },
	{ "albumsearch",	c_albumsearch,		args_string,
		"search albums matching substring" },
	{ "albumsartist",	c_albumsartist,		args_artist,
		"search albums by artist" },
	// TODO: albumalter/-add

	{ "artistlist",		c_artistlist,		args_none,
		"show all artists" },
	{ "artistget",		c_artistget,		args_idlist,
		"show artist details" },
	{ "artistsearch",	c_artistsearch,		args_string,
		"search artists matching substring" },
	{ "artistsetname",	c_artistsetname,	args_artistname,
		"change artist name" },
	{ "artistmerge",	c_artistmerge,		args_artistmerge,
		"merge two artists" },

	{ "sfilterlist",	c_sfilterlist,		args_none,
		"show all saved filters" },
	{ "sfilterget",		c_sfilterget,		args_idlist,
		"show saved filter details" },
	{ "sfilteradd",		c_sfilteradd,		args_name,
		"add new slot for saving filters" },
	{ "sfiltersetname",	c_sfiltersetname,	args_sfiltername,
		"rename saved filter" },
	{ "sfiltersetfilter",	c_sfiltersetfilter,	args_sfilterfilter,
		"save filter to slot" },
	{ "sfilterload",	c_sfilterload,		args_sfilter,
		"load and set ilter from slot" },
	{ "sfiltersave",	c_sfiltersave,		args_sfilter,
		"save current filter to slot" },


	{ NULL,			NULL,			args_none,
		NULL },
};

GENC(lst_cmd)
{
	static t_cmd *next = NULL;
	static int len;

	// TODO: only offer commands, the user is permitted to use
	(void)dudl;

	if( state == 0 ){
		next = cmds_top;
		len = strlen(text);
	}


	while( next && next->name ){
		if( 0 == strncasecmp(next->name, text, len))
			return strdup(next++->name);
		next++;
	}

	return NULL;
}

static t_cmd *cmd_find( char *name )
{
	t_cmd *cmd;

	for( cmd = cmds_top; cmd && cmd->name; cmd++ ){
		if( 0 == strcmp(cmd->name, name))
			return cmd;
	}

	return NULL;
}

static char *cmd_usage( t_cmd *cmd )
{
	t_cmd_arg *arg;
	char buf[2048] = "";

	strcat( buf, cmd->name );

	for( arg = cmd->args; arg && arg->name; arg++ ){
		strcat( buf, " <" );
		strcat( buf, arg->name);
		strcat( buf, ">" );
	}

	return strdup(buf);
}

CMD(c_help)
{
	t_cmd *cmd;

	(void)dudl;
	(void)argv;

	for( cmd = cmds_top; cmd && cmd->name; cmd++ ){
		char *usage;

		if( NULL == (usage = cmd_usage(cmd)))
			continue;

		dmsg_msg( "%s\n\t%s\n", usage, cmd->help );
		free(usage);
	}

	return 0;
}

CMD(c_helpcmd)
{
	char *name = (char*)argv[0];
	t_cmd *cmd;
	char *usage;

	(void)dudl;

	if( NULL == (cmd = cmd_find( name )))
		return 0;


	if( NULL == (usage = cmd_usage(cmd)))
		return 0;

	dmsg_msg( "%s\n\t%s\n", usage, cmd->help );
	free(usage);

	return 0;
}

static void cmd_arg_free( t_cmd *cmd, void **argv )
{
	void **ndat;
	t_cmd_arg *narg;

	for( ndat = argv, narg = cmd->args; *ndat; ndat++, narg++ ){
		//assert(narg);

		if( narg->free )
			(*narg->free)(*ndat);
	}
	free(argv);
}

static int cmd_parse( dudlc *dudl, t_cmd *cmd, char *line )
{
	char *next = line;
	char *end;
	t_cmd_arg *arg;
	int argc = 0;
	void **argv = NULL;
	int missing = 0;

	if( NULL == (argv = malloc(sizeof(void*))))
		return -1;
	argv[0] = NULL;

	for( arg = cmd->args; arg && arg->name; arg++ ){
		void *data;
		void **tmp;

		SKIPSPACE(next);

		if( *next == 0 ){
			missing++;
			continue;
		}

		data = (*arg->parse)( dudl, next, &end );
		// TODO respect ->optional
		if( end == next ){
			dmsg_msg( "invalid data for argument %s\n", arg->name );
			goto clean1;
		}
		next = end;

		if( NULL == (tmp = realloc(argv, (argc +2) * sizeof(void*)))){
			goto clean1;
		}

		argv = tmp;
		argv[argc++] = (void*)data;
		argv[argc] = NULL;

	}


	if( *next != 0 ){
		dmsg_msg( "too many arguments\n" );
		goto clean1;
	}

	if( missing ){
		char *usage;

		if( NULL != (usage = cmd_usage(cmd))){
			dmsg_msg( "usage: %s\n", usage );
			free(usage);
		}

		goto clean1;
	}

	if( (*cmd->run)( dudl, argv ) ){
		if( 0 > duc_fd(dudl)){
			dmsg_msg( "failed: connection error\n" );
		} else {
			dmsg_msg( "failed: %s\n", duc_rmsg(dudl) );
		}
	}

	cmd_arg_free(cmd, argv);
	return 0;
	
clean1:
	cmd_arg_free(cmd, argv);
	return -1;

}

static duc_cgen cmd_find_genc( dudlc *dudl, t_cmd *cmd, char *line, unsigned int pos )
{
	char *next = line;
	char *end;
	t_cmd_arg *arg;
	int argc = 0;
	void **argv = NULL;

	if( NULL == (argv = malloc(sizeof(void*))))
		return NULL;
	argv[0] = NULL;

	for( arg = cmd->args; arg && arg->name; arg++ ){
		void *data;
		void **tmp;

		SKIPSPACE(next);
		if( *next == 0 )
			break;

		if( (unsigned int) (next - line) >= pos )
			break;

		data = (*arg->parse)( dudl, next, &end );
		// TODO respect ->optional
		if( end == next )
			goto clean1;
		next = end;

		if( NULL == (tmp = realloc(argv, (argc +2) * sizeof(void*)))){
			goto clean1;
		}

		argv = tmp;
		argv[argc++] = (void*)data;
		argv[argc] = NULL;

	}

	if( ! arg || ! arg->name )
		goto clean1;

	cmd_arg_free(cmd, argv);

	return arg->genc;
	
clean1:
	cmd_arg_free(cmd, argv);
	return NULL;
}

int duc_cmd( dudlc *dudl, char *line )
{
	char *end;
	char *scmd;
	t_cmd *cmd;

	SKIPSPACE(line);
	scmd = val_name( dudl, line, &end );
	if( line == end || ! scmd ){
		dmsg_msg( "no such  command\n" );
		return -1;
	}
	line = end;

	if( NULL == (cmd = cmd_find( scmd ))){
		dmsg_msg( "no such command\n" );
		return -1;
	}

	return cmd_parse( dudl, cmd, line );
}

duc_cgen duc_cgen_find( dudlc *dudl, char* line, unsigned int pos )
{
	char *start = line;
	char *end;
	char *scmd;
	t_cmd *cmd;

	if( pos == 0 )
		return lst_cmd;

	SKIPSPACE(start);
	scmd = val_name( dudl, start, &end );
	if( start == end || ! scmd ){
		return NULL;
	}
	pos -= end - line;
	start = end;

	if( NULL == (cmd = cmd_find( scmd )))
		return NULL;

	return cmd_find_genc( dudl, cmd, start, pos );
}
