#define _GNU_SOURCE

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "parseval.h"

typedef int (*t_convert)( dudlc *dudl, char *in, char **end );


/*
 * list := <item>[,...]
 * item := <parsed by t_convert function>
 */
static int idl_addclist( t_idlist *idl, char *in, char **end, dudlc *con, t_convert conv )
{
	int added = 0;
	char *ntok = in;
	char *e;

	while(1){
		int id;
		
		id = (*conv)( con, ntok, &e );
		if( ntok == e ){
			if( end )
				*end = in;
			return 0;
		}
		idl_add( idl, id );
		added ++;

		if( *e != ',' )
			break;

		++e;
		ntok = e;
	}

	if( end )
		*end = e;

	return added;
}

static int idl_addalbum( t_idlist *idl, dudlc *con, int album, int first, int last )
{
	duc_it_track *it;
	duc_track *t;
	int added = 0;

	if( album < 0 ){
		duc_track *t;
		if(NULL == (t = duc_cmd_curtrack(con)))
			return 0;

		album = t->album->id;
		duc_track_free(t);
	}

        if( NULL == (it = duc_cmd_tracksalbum(con,album)))
		return 0;

	for( t = duc_it_track_cur(it); t; t = duc_it_track_next(it)){
		if( first < 0 || last <= first || 
				(first <= t->albumnr && last >= t->albumnr)){
			idl_add(idl, t->id );
		}
		duc_track_free(t);
	}
	duc_it_track_done(it);

	return added;
}

int val_bool( dudlc *con, char *in, char **end ) // TODO**
{
	(void)con;
	(void)end;

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

int val_uint( dudlc *con, char *in, char **end )
{
	(void)con;
	return strtoul(in, end, 10 );
}

char *val_string( dudlc *con, char *in, char **end )
{
	(void)con;

	if( end )
		*end = in + strlen(in);

	return strdup(in);
}

char *val_name( dudlc *con, char *in, char **end )
{
	char *e = in;

	(void)con;
	if( end ) *end = in;

	while(isalpha(*e))
		e++;

	if( e == in )
		return NULL;

	if( end )
		*end = e;

	return strndup(in, e - in);
}

t_idlist *val_idlist( dudlc *con, char *in, char **end )
{
	t_idlist *idl;

	(void)con;
	if( NULL == (idl = idl_new(NULL)))
		return NULL;

	idl_addlist( idl, in, end );

	return idl;
}

int val_user( dudlc *con, char *in, char **end ) // TODO*
{
	return val_uint( con, in, end );
}

t_idlist *val_userlist( dudlc *con, char *in, char **end )
{
	t_idlist *idl;

	if( NULL == (idl = idl_new(NULL)))
		return NULL;

	idl_addclist( idl, in, end, con, val_user );

	return idl;
}

int val_right( dudlc *con, char *in, char **end ) // TODO
{
	return val_uint( con, in, end );
}

/* 
 * track := 'c'|<id>|<aid>/<pos>
 */
int val_track( dudlc *con, char *in, char **end )
{
	int a;
	int b;
	char *s, *e;
	duc_track *t;

	if( end ) *end = in;

	if( *in == 'c' ){
		if(NULL == (t = duc_cmd_curtrack(con))){
			return -1;
		}
		a = t->id;
		duc_track_free(t);

		if( end ) (*end)++;
		return a;
	}

	a = strtoul( in, &e, 10 );
	if( e == in )
		return -1;

	if( *e != '/' ){
		if( end ) *end = e;
		return a;
	}

	s = e + 1;
	b = strtoul( s, &e, 10 );
	if( e == s )
		return -1;

	if( end ) *end = e;
	return duc_cmd_track2id(con, a, b );
}

/*
 * list := <item>[,...]
 * item := c|<id>|c/<pos>
 * pos  := *|<num>[-<num2>]
 */
t_idlist *val_tracklist( dudlc *con, char *in, char **end)
{
	t_idlist *idl;
	char *tok = in;

	if( end ) *end = in;

	if( NULL == (idl = idl_new(NULL)))
		return NULL;

	while(1){
		char *e;
		int id1;

		id1 = strtoul(tok, &e, 10 );
		if( e == tok ){
			if( *e == 'c' ){
				id1 = -1;
				e++;
			} else {
				idl_done(idl);
				return NULL;
			}
		}
		tok = e;

		/* is it <id1>/anything? */
		if( *tok == '/' ){
			int pos1;

			tok++;
			pos1 = strtoul(tok, &e, 10 );
			if( e == tok ){
				/* no <pos>, must be '*' */
				if( e[0] == '*' && (e[1] == 0 || e[1] == ',')){
					idl_addalbum(idl, con, id1, -1, -1 );
					tok++;
				} else {
					idl_done(idl);
					return 0;
				}

			} else {
				tok = e;

				/* is it <id1>/<pos1>-anything? */
				if( *tok == '-' ){
					int pos2;

					tok++;
					pos2 = strtoul(tok, &e, 10 );
					if( e == tok ){
						idl_done(idl);
						return 0;
					}
					tok = e;

					idl_addalbum(idl, con, id1, pos1, pos2);
				} else {
					int tid;
					tid = duc_cmd_track2id(con, id1, pos1 );
					if( tid >= 0 )
						idl_add( idl, tid );
				}


			}

		} else if( *tok == 0 || *tok == ',' ){
			/* no / found */
			if( id1 < 0 ){
				duc_track *t;
				if(NULL == (t = duc_cmd_curtrack(con))){
					idl_done(idl);
					return 0;
				}

				id1 = t->id;
				duc_track_free(t);
			}
			idl_add(idl,id1);

		} else {
			idl_done(idl);
			return 0;
		}

		if( *tok != ',' )
			break;
		tok++;
	}

	if( end )
		*end = tok;

	return idl;
}

/*
 * tag := <tagname>|<id>
 */
int val_tag( dudlc *con, char *in, char **end )
{
	int id;
	char *e;
	char tag[10];
	int len;

	if( end ) *end = in;

	id = strtoul( in, &e, 10 );
	if( in != e ){
		if( end ) *end = e;
		return id;
	}

	len = strcspn(in, "\t ");
	if( len >= 10  )
		return -1;

	e = in +len;
	strncpy( tag, in, len );
	tag[len] = 0;

	if( 0 > ( id = duc_cmd_tag2id( con, tag ))){
		return -1;
	}

	if( end ) *end = e;
	return id;
}

t_idlist *val_taglist( dudlc *con, char *in, char **end )
{
	t_idlist *idl;

	if( NULL == (idl = idl_new(NULL)))
		return NULL;

	idl_addclist( idl, in, end, con, val_tag );

	return idl;
}

/*
 * sfilter := <sfiltername>|<id>
 */
int val_sfilter( dudlc *con, char *in, char **end ) // TODO
{
	int id;
	char *e;
	char sfilter[10];
	int len;

	if( end ) *end = in;

	id = strtoul( in, &e, 10 );
	if( in != e ){
		if( end ) *end = e;
		return id;
	}

	len = strcspn(in, "\t ");
	if( len >= 10  )
		return -1;

	e = in +len;
	strncpy( sfilter, in, len );
	sfilter[len] = 0;

	if( 0 > ( id = duc_cmd_sfilter2id( con, sfilter ))){
		return -1;
	}

	if( end ) *end = e;
	return id;
}

