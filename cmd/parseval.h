#ifndef _PARSEVAL_H
#define _PARSEVAL_H

#include <dudlc.h>
#include "idlist.h"

typedef struct _t_enum {
	char *text;
	int val;
} t_enum;

extern t_enum bools[];
extern t_enum rights[];

int val_bool( dudlc *con, char *in, char **end );
int val_time( dudlc *con, char *in, char **end );
int val_uint( dudlc *con, char *in, char **end );
int val_year( dudlc *con, char *in, char **end );
char *val_string( dudlc *con, char *in, char **end );
char *val_name( dudlc *con, char *in, char **end );
t_idlist *val_idlist( dudlc *con, char *in, char **end );

int val_user( dudlc *con, char *in, char **end );
t_idlist *val_userlist( dudlc *con, char *in, char **end );

int val_right( dudlc *con, char *in, char **end );

int val_track( dudlc *con, char *in, char **end );
t_idlist *val_tracklist( dudlc *con, char *in, char **end );

int val_tag( dudlc *con, char *in, char **end );
t_idlist *val_taglist( dudlc *con, char *in, char **end );

int val_sfilter( dudlc *con, char *in, char **end );

int val_rgtype( dudlc *con, char *in, char **end );
int val_double( dudlc *con, char *in, char **end );


#endif
