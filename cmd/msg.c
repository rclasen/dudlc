

#include <stdlib.h>
#include <stdarg.h>

#include "dudlccmd.h"

t_dmsg dmsg_msg_cb = NULL;

int dmsg_msg( const char *fmt, ... )
{
	va_list ap;

	if( !dmsg_msg_cb )
		return 0;

	va_start( ap, fmt );
	return (*dmsg_msg_cb)(fmt, ap );
}

void dmsg_dump_clients( duc_it_client *it )
{
	char buf[BUFLENCLIENT];
	duc_client *c;

	dmsg_msg( "%s\n\n", dfmt_clienthead(buf, BUFLENCLIENT));
	for( c = duc_it_client_cur(it); c; c = duc_it_client_next(it)){
		dmsg_msg( "%s\n", dfmt_client(buf, BUFLENCLIENT, c ));
		duc_client_free(c);
	}
}

void dmsg_dump_tracks( duc_it_track *it )
{
	duc_track *t;
	char buf[BUFLENTRACK];

	dmsg_msg( "%s\n\n", dfmt_trackhead(buf, BUFLENTRACK));
	for( t = duc_it_track_cur(it); t; t = duc_it_track_next(it)){
		dmsg_msg( "%s\n", dfmt_track(buf,BUFLENTRACK,t));
		duc_track_free(t);
	}
}

void dmsg_dump_queue( duc_it_queue *it )
{
	duc_queue *t;
	char buf[BUFLENQUEUE];

	dmsg_msg( "%s\n\n", dfmt_queuehead(buf, BUFLENQUEUE));
	for( t = duc_it_queue_cur(it); t; t = duc_it_queue_next(it)){
		dmsg_msg( "%s\n", dfmt_queue(buf,BUFLENQUEUE,t));
		duc_queue_free(t);
	}
}


void dmsg_dump_history( duc_it_history *it )
{
	duc_history *t;
	char buf[BUFLENQUEUE];

	dmsg_msg( "%s\n\n", dfmt_historyhead(buf, BUFLENQUEUE));
	for( t = duc_it_history_cur(it); t; t = duc_it_history_next(it)){
		dmsg_msg( "%s\n", dfmt_history(buf,BUFLENQUEUE,t));
		duc_history_free(t);
	}
}


void dmsg_dump_tags( duc_it_tag *it )
{
	duc_tag *t;
	char buf[BUFLENTAG];

	dmsg_msg( "%s\n\n", dfmt_taghead(buf, BUFLENTAG));
	for( t = duc_it_tag_cur(it); t; t = duc_it_tag_next(it)){
		dmsg_msg( "%s\n", dfmt_tag(buf,BUFLENTAG,t));
		duc_tag_free(t);
	}
}

void dmsg_dump_users( duc_it_user *it )
{
	duc_user *t;
	char buf[BUFLENUSER];

	dmsg_msg( "%s\n\n", dfmt_userhead(buf, BUFLENUSER));
	for( t = duc_it_user_cur(it); t; t = duc_it_user_next(it)){
		dmsg_msg( "%s\n", dfmt_user(buf,BUFLENUSER,t));
		duc_user_free(t);
	}
}

void dmsg_dump_albums( duc_it_album *it )
{
	duc_album *t;
	char buf[BUFLENALBUM];

	dmsg_msg( "%s\n\n", dfmt_albumhead(buf, BUFLENALBUM));
	for( t = duc_it_album_cur(it); t; t = duc_it_album_next(it)){
		dmsg_msg( "%s\n", dfmt_album(buf,BUFLENALBUM,t));
		duc_album_free(t);
	}
}

void dmsg_dump_artists( duc_it_artist *it )
{
	duc_artist *t;
	char buf[BUFLENARTIST];

	dmsg_msg( "%s\n\n", dfmt_artisthead(buf, BUFLENARTIST));
	for( t = duc_it_artist_cur(it); t; t = duc_it_artist_next(it)){
		dmsg_msg( "%s\n", dfmt_artist(buf,BUFLENARTIST,t));
		duc_artist_free(t);
	}
}



