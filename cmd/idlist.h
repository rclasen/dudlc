#ifndef _IDLIST_H
#define _IDLIST_H

#include <unistd.h>

typedef struct _t_idlist {
	int *ids;
	size_t num;
	size_t cur;
} t_idlist;

t_idlist *idl_new( t_idlist *src );
int idl_add( t_idlist *idl, int id );
void idl_done( t_idlist *idl );

int idl_cur( t_idlist *idl );
int idl_begin( t_idlist *idl );
int idl_next( t_idlist *idl );

int idl_addlist( t_idlist *idl, char *list, char **end );

#endif
