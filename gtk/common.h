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
/* bind this to POS column, album_id has to be in previous column: */
gint sortfunc_albumpos( GtkTreeModel *model,
                          GtkTreeIter  *a,
                          GtkTreeIter  *b,
                          gpointer      data);

/*
 * cellfunc
 */

void cellfunc_trackid( 
	GtkTreeViewColumn *col,
	GtkCellRenderer *cell,
	GtkTreeModel *model,
	GtkTreeIter *iter,
	gpointer data );

void cellfunc_duration( 
	GtkTreeViewColumn *col,
	GtkCellRenderer *cell,
	GtkTreeModel *model,
	GtkTreeIter *iter,
	gpointer data );

void cellfunc_time( 
	GtkTreeViewColumn *col,
	GtkCellRenderer *cell,
	GtkTreeModel *model,
	GtkTreeIter *iter,
	gpointer data );

/*
 * childwindow.c
 */

int toggle_win_on_delete( GtkWidget *widget, GdkEvent  *event, GtkAction *action );
GtkWidget *childwindow_new( const char *title, GtkWidget *contents, GtkUIManager *ui, const char *uidesc );
GtkWidget *childscroll_new( const char *title, GtkWidget *contents, GtkUIManager *ui, const char *uidesc );

/*
 * tracksearch.c
 */
GtkWidget *tracksearch_window( void );


/*
 * context.c
 */

typedef gint (*context_populate_func)( GtkWidget *view, GtkWidget *menu );
void context_add( GtkTreeView *view, GtkMenu *menu );

/*
 * treehelper
 */

gint tree_select_count( GtkTreeSelection *sel );

void tree_view_select( GtkTreeView *view, int col, int selid );
gint tree_view_select_count( GtkTreeView *list );
void tree_view_column_on_clicked( GtkWidget *col, GtkTreeView *view );

/*
 * track.c
 */

enum {
	TRACKLIST_ID,
	TRACKLIST_ALBUM_ID,
	TRACKLIST_ALBUM_POS,
	TRACKLIST_ALBUM,
	TRACKLIST_DURATION,
	TRACKLIST_ARTIST_ID,
	TRACKLIST_ARTIST,
	TRACKLIST_TITLE,
	TRACKLIST_N_COLUMNS,
};

GtkWidget *track_list_new( gboolean full );
GtkWidget *track_list_new_with_list( gboolean full, duc_it_track *in );
void track_list_populate( GtkTreeView *view, duc_it_track *in );
void track_list_clear( GtkTreeView *view );
void track_list_select( GtkTreeView *view, int id );
void track_list_select_queueadd( GtkTreeView *list );
void track_list_select_queuealbum( GtkTreeView *list );

/*
 * album.c
 */

enum {
	ALBUMLIST_ID,
	ALBUMLIST_YEAR,
	ALBUMLIST_ARTIST_ID,
	ALBUMLIST_ARTIST,
	ALBUMLIST_ALBUM,
	ALBUMLIST_N_COLUMNS,
};

GtkWidget *album_list_new( gboolean full );
GtkWidget *album_list_new_with_list( gboolean full, duc_it_album *in );
void album_list_populate( GtkTreeView *view, duc_it_album *in );
void album_list_clear( GtkTreeView *view );
void album_list_select( GtkTreeView *view, int id );
void album_list_select_queuealbum( GtkTreeView *list );

/*
 * artist.c
 */

enum {
	ARTISTLIST_ID,
	ARTISTLIST_ARTIST,
	ARTISTLIST_N_COLUMNS,
};

GtkWidget *artist_list_new( void );
GtkWidget *artist_list_new_with_list( duc_it_artist *in );
void artist_list_populate( GtkTreeView *view, duc_it_artist *in );
void artist_list_select( GtkTreeView *view, int id );

/*
 * queue.c
 */

enum {
	QUEUELIST_ID,
	QUEUELIST_USER_ID,
	QUEUELIST_USER,
	QUEUELIST_QUEUED,
	QUEUELIST_ALBUM_ID,
	QUEUELIST_ALBUM_POS,
	QUEUELIST_ALBUM,
	QUEUELIST_DURATION,
	QUEUELIST_ARTIST_ID,
	QUEUELIST_ARTIST,
	QUEUELIST_TRACK_ID,
	QUEUELIST_TITLE,
	QUEUELIST_N_COLUMNS,
};

GtkWidget *queue_list_new( void );
void queue_list_populate( GtkTreeView *view, duc_it_queue *in );
void queue_list_add( GtkTreeView *view, duc_queue *item );
void queue_list_del( GtkTreeView *view, int queueid );
void queue_list_clear( GtkTreeView *view );

void queue_list_store_add( GtkTreeModel *store, duc_queue *queue );
void queue_list_store_del( GtkTreeModel *store, int queueid );

void queue_list_select_deltrack( GtkTreeView *list );

/*
 * queuewin.c
 */

GtkWidget *queuewin_new( GtkAction *action );
void queuewin_populate( duc_it_queue *queue );
void queuewin_clear( void );
void queuewin_add( duc_queue *item );
void queuewin_del( int id );

/*
 * curalbums.c
 */

GtkWidget *curalbums_new( GtkAction *action );
void curalbums_populate( duc_it_album *albums );
void curalbums_clear( void );

/*
 * curtracks.c
 */

GtkWidget *curtracks_new( GtkAction *action );
void curtracks_populate( duc_it_track *tracks );
void curtracks_clear( void );

/*
 * browse.c
 */

GtkWidget *browse_window( gint artist, gint album, gint track );

#endif //_COMMON_H
