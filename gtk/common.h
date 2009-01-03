/*
 * Copyright (c) 2008 Rainer Clasen
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

#ifndef _COMMON_H
#define _COMMON_H

#include <gtk/gtk.h>
#include <dudlc.h>

/*
 * main.c
 */

extern dudlc *con;

/*
 * options.c
 */

typedef struct {
	char *host;
	int port;
	char *user;
	char *pass;
} options;

void options_init( options *opt );
int options_read( options *opt, const char *fname );

/*
 * playbox.c
 */

GtkWidget *playbox_new( void );
void playbox_buttons_show( gboolean show );
void playbox_buttons_enable( duc_playstatus stat );
void playbox_detail_update( duc_track *t );
void playbox_detail_clear( void );
void playbox_progress_update( int r );

/*
 * sortfunc.c
 */
gint sortfunc_int( GtkTreeModel *model,
                          GtkTreeIter  *a,
                          GtkTreeIter  *b,
                          gpointer      data);
gint sortfunc_str( GtkTreeModel *model,
                          GtkTreeIter  *a,
                          GtkTreeIter  *b,
                          gpointer      data);

/*
 * childwindow.c
 */

GtkWidget *childwindow_new( const char *title, GtkWidget *contents );

/*
 * track.c
 */

GtkWidget *track_list_new( void );
GtkWidget *track_list_new_with_list( duc_it_track *in );
//void track_list_populate( GtkWidget *list, duc_it_track *in );
//gint track_list_select_count( GtkWidget *list );
//void track_list_select_queueadd( GtkWidget *list );

#endif //_COMMON_H
