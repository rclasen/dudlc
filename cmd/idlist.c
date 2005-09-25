#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "idlist.h"

void idl_done( t_idlist *idl )
{
	free(idl->ids);
	free(idl);
}

t_idlist *idl_new( t_idlist *src )
{
	t_idlist *idl;

	if( NULL == (idl = malloc(sizeof(t_idlist))))
		return NULL;

	memset(idl,0,sizeof(t_idlist));

	if( src && src->num ){
		if( NULL == (idl->ids = malloc(src->num * sizeof(int*)))){
			free(idl);
			return NULL;
		}
		memcpy(idl->ids, src->ids, src->num * sizeof(int*));
		idl->num = src->num;
	}

	return idl;
}

int idl_add( t_idlist *idl, int id )
{
	int *tmp;

	if( NULL == (tmp = realloc(idl->ids, (idl->num +1) * sizeof(int*))))
		return -1;

	idl->ids = tmp;
	idl->ids[idl->num++] = id;
	return 0;
}

/*
 * list := <item>[','<...>]
 * item := <from>['-'<to>]
 */
int idl_addlist( t_idlist *idl, char *list, char **end )
{
	char *tok = list;
	int added = 0;
	
	if( end )
		*end = list;

	while( isdigit(*tok) ){
		char *e;
		int id1;

		id1 = strtoul( tok, &e, 10 );
		if( e == tok )
			return 0;

		tok = e;
		if( *tok == '-' ){
			int id2, id;

			tok++;
			id2 = strtoul(tok, &e, 10);
			if( e == tok )
				return 0;
			tok = e;

			if( id2 <= id1 )
				return 0;

			for( id = id1; id <= id2; ++id ){
				if( -1 == idl_add(idl, id ) )
					return 0;

				added++;
			}

		} else {
			if( -1 == idl_add(idl, id1 ))
				return 0;
			added++;
		}

		if( *tok == ',' )
			tok++;
	}

	if( end )
		*end = tok;

	return added;
}


int idl_cur( t_idlist *idl )
{
	if( idl->cur < idl->num )
		return idl->ids[idl->cur];
	return 0;
}

int idl_begin( t_idlist *idl )
{
	idl->cur = 0;
	return idl_cur(idl);
}

int idl_next( t_idlist *idl )
{
	idl->cur++;
	return idl_cur(idl);
}

