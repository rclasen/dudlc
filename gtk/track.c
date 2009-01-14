/*
 * Copyright (c) 2008 Rainer Clasen
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

/* list tracks (filter, tracksalbum, tracksartist) */

#include "common.h"

/*
 * helper for selection processing
 */

static void track_list_each_queueadd( 
	GtkTreeModel  *model,
	GtkTreePath   *path,
	GtkTreeIter   *iter,
	gpointer       data)
{
	gint trackid;

	(void)path;
	(void)data;
	gtk_tree_model_get (model, iter, TRACKLIST_ID, &trackid, -1);
	duc_cmd_queueadd( con, trackid );
}

static void track_list_each_queuealbum( 
	GtkTreeModel  *model,
	GtkTreePath   *path,
	GtkTreeIter   *iter,
	gpointer       data)
{
	gint trackid;

	(void)path;
	(void)data;
	gtk_tree_model_get (model, iter, TRACKLIST_ALBUM_ID, &trackid, -1);
	duc_cmd_queuealbum( con, trackid );
}

/*
 * selection processing
 */

void track_list_select_queueadd( GtkTreeView *list )
{
	gtk_tree_selection_selected_foreach(
		gtk_tree_view_get_selection(list),
		track_list_each_queueadd, NULL);
}

void track_list_select_queuealbum( GtkTreeView *list )
{
	gtk_tree_selection_selected_foreach(
		gtk_tree_view_get_selection(GTK_TREE_VIEW(list)),
		track_list_each_queuealbum, NULL);
}

/*
 * context menu
 */
static void track_list_context_qtrack_on_activate(GtkWidget *menuitem, gpointer data)
{
	(void)menuitem;
	track_list_select_queueadd(GTK_TREE_VIEW(data));
}

static void track_list_context_qalbum_on_activate(GtkWidget *menuitem, gpointer data)
{
	(void)menuitem;
	track_list_select_queuealbum(GTK_TREE_VIEW(data));
}

static gint track_list_context_populate( GtkWidget *view, GtkWidget *menu )
{
	gint selected;
	GtkWidget *menuitem;
	gint numitems = 0;


	if( 0 >= (selected = tree_view_select_count(GTK_TREE_VIEW(view))))
		return 0;


	menuitem = gtk_menu_item_new_with_label( selected > 1 
		? "queue tracks"
		: "queue track" );
	g_signal_connect(menuitem, "activate",
		(GCallback)track_list_context_qtrack_on_activate, view);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);
	numitems++;

	if( selected == 1 ){
		menuitem = gtk_menu_item_new_with_label("queue album");
		g_signal_connect(menuitem, "activate",
			(GCallback)track_list_context_qalbum_on_activate, view);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);
		numitems++;

		/* TODO: context list artist albums*/
		/* TODO: context list artist tracks*/
		/* TODO: context list albums tracks */

		/* TODO: context filter artist */
		/* TODO: context filter album */
	}

	return numitems;
}

/*
 * the list view
 */

GtkWidget *track_list_new( gboolean full )
{
	GtkTreeView *view;
	GtkTreeViewColumn *col;
	GtkCellRenderer *renderer;

	view = GTK_TREE_VIEW(gtk_tree_view_new());

	context_add( view, track_list_context_populate );



	renderer = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new();
	g_object_set_data( G_OBJECT(col), "columnum", (gpointer)TRACKLIST_ALBUM_POS);
	g_signal_connect(col, "clicked", (GCallback)tree_view_column_on_clicked, view );
	gtk_tree_view_column_set_title( col, "Id" );
	gtk_tree_view_column_pack_start( col, renderer, TRUE );
	gtk_tree_view_column_set_cell_data_func( col, renderer, 
		cellfunc_trackid, (gpointer)TRACKLIST_ALBUM_POS, NULL );
	gtk_tree_view_column_set_resizable( col, TRUE );
	gtk_tree_view_column_set_sort_column_id(col, TRACKLIST_ALBUM_POS);
	gtk_tree_view_append_column( view, col );
	
	renderer = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new();
	g_object_set_data( G_OBJECT(col), "columnum", (gpointer)TRACKLIST_DURATION);
	g_signal_connect(col, "clicked", (GCallback)tree_view_column_on_clicked, view );
	gtk_tree_view_column_set_title( col, "Duration" );
	gtk_tree_view_column_pack_start( col, renderer, TRUE );
	gtk_tree_view_column_set_cell_data_func( col, renderer, 
		cellfunc_duration, (gpointer)TRACKLIST_DURATION, NULL );
	gtk_tree_view_column_set_resizable( col, TRUE );
	gtk_tree_view_column_set_sort_column_id(col, TRACKLIST_DURATION);
	gtk_tree_view_append_column( view, col );

	if( full ){ /* TODO: select columns */

	renderer = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new_with_attributes(
			"Artist", renderer, "text", TRACKLIST_ARTIST, NULL );
	g_object_set_data( G_OBJECT(col), "columnum", (gpointer)TRACKLIST_ARTIST);
	g_signal_connect(col, "clicked", (GCallback)tree_view_column_on_clicked, view );
	gtk_tree_view_column_set_resizable( col, TRUE );
	gtk_tree_view_column_set_sort_column_id(col, TRACKLIST_ARTIST);
	gtk_tree_view_append_column( view, col );

	renderer = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new_with_attributes(
			"Album", renderer, "text", TRACKLIST_ALBUM, NULL );
	g_object_set_data( G_OBJECT(col), "columnum", (gpointer)TRACKLIST_ALBUM);
	g_signal_connect(col, "clicked", (GCallback)tree_view_column_on_clicked, view );
	gtk_tree_view_column_set_resizable( col, TRUE );
	gtk_tree_view_column_set_sort_column_id(col, TRACKLIST_ALBUM);
	gtk_tree_view_append_column( view, col );
	
	}
	
	renderer = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new_with_attributes(
			"Title", renderer, "text", TRACKLIST_TITLE, NULL );
	g_object_set_data( G_OBJECT(col), "columnum", (gpointer)TRACKLIST_TITLE);
	g_signal_connect(col, "clicked", (GCallback)tree_view_column_on_clicked, view );
	gtk_tree_view_column_set_resizable( col, TRUE );
	gtk_tree_view_column_set_sort_column_id(col, TRACKLIST_TITLE);
	gtk_tree_view_append_column( view, col );
	
	/* TODO: add column with tags of track */
	
	gtk_tree_view_set_search_column( GTK_TREE_VIEW(view), TRACKLIST_ALBUM_POS );
	gtk_widget_show(GTK_WIDGET(view));

	return GTK_WIDGET(view);
}

GtkWidget *track_list_new_with_list( gboolean full, duc_it_track *in )
{
	GtkWidget *view;

	if( NULL == (view = track_list_new(full))) 
		return NULL;

	track_list_populate( GTK_TREE_VIEW(view), in );

	return view;
}

void track_list_populate( GtkTreeView *view, duc_it_track *in )
{
	GtkTreeModel *store;
	GtkTreeIter add;
	duc_track *track;

	store = GTK_TREE_MODEL(gtk_list_store_new( TRACKLIST_N_COLUMNS,
		G_TYPE_INT,
		G_TYPE_INT,
		G_TYPE_INT,
		G_TYPE_STRING,
		G_TYPE_INT,
		G_TYPE_INT,
		G_TYPE_STRING,
		G_TYPE_STRING ) );

	for( track = duc_it_track_cur(in); track; track = duc_it_track_next(in)){
		gchar *album, *title, *artist;

		album = g_locale_to_utf8( track->album->album, -1, NULL, NULL, NULL);
		title = g_locale_to_utf8( track->title, -1, NULL, NULL, NULL);
		artist = g_locale_to_utf8( track->artist->artist, -1, NULL, NULL, NULL);

		gtk_list_store_append( GTK_LIST_STORE(store), &add );
		gtk_list_store_set( GTK_LIST_STORE(store), &add, 
			TRACKLIST_ID, track->id,
			TRACKLIST_ALBUM_ID, track->album->id,
			TRACKLIST_ALBUM_POS, track->albumnr,
			TRACKLIST_ALBUM, album,
			TRACKLIST_DURATION, track->duration,
			TRACKLIST_ARTIST_ID, track->artist->id,
			TRACKLIST_ARTIST, artist,
			TRACKLIST_TITLE, title,
			-1);

		g_free(album);
		g_free(title);
		g_free(artist);
		duc_track_free(track);
	}

	gtk_tree_sortable_set_sort_func( GTK_TREE_SORTABLE(store),
		TRACKLIST_ID, sortfunc_int, (gpointer)TRACKLIST_ID, NULL);

	gtk_tree_sortable_set_sort_func( GTK_TREE_SORTABLE(store),
		TRACKLIST_ALBUM_ID, sortfunc_int, (gpointer)TRACKLIST_ALBUM_ID, NULL);

	gtk_tree_sortable_set_sort_func( GTK_TREE_SORTABLE(store),
		TRACKLIST_ALBUM_POS, sortfunc_albumpos, (gpointer)TRACKLIST_ALBUM_POS, NULL);

	gtk_tree_sortable_set_sort_func( GTK_TREE_SORTABLE(store),
		TRACKLIST_ALBUM, sortfunc_str, (gpointer)TRACKLIST_ALBUM, NULL);

	gtk_tree_sortable_set_sort_func( GTK_TREE_SORTABLE(store),
		TRACKLIST_DURATION, sortfunc_int, (gpointer)TRACKLIST_DURATION, NULL);

	gtk_tree_sortable_set_sort_func( GTK_TREE_SORTABLE(store),
		TRACKLIST_ARTIST_ID, sortfunc_int, (gpointer)TRACKLIST_ARTIST_ID, NULL);

	gtk_tree_sortable_set_sort_func( GTK_TREE_SORTABLE(store),
		TRACKLIST_ARTIST, sortfunc_str, (gpointer)TRACKLIST_ARTIST, NULL);

	gtk_tree_sortable_set_sort_func( GTK_TREE_SORTABLE(store),
		TRACKLIST_TITLE, sortfunc_str, (gpointer)TRACKLIST_TITLE, NULL);

	gtk_tree_view_set_model( GTK_TREE_VIEW(view), GTK_TREE_MODEL(store) );
	g_object_unref(store);
}

