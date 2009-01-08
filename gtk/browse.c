/*
 * Copyright (c) 2008 Rainer Clasen
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

#include "common.h"

static void browse_artist_each_getid( 
	GtkTreeModel  *model,
	GtkTreePath   *path,
	GtkTreeIter   *iter,
	gpointer	*id )
{
	(void)path;
	gtk_tree_model_get (model, iter, ARTISTLIST_ID, id, -1);
}

static void browse_album_each_getid( 
	GtkTreeModel  *model,
	GtkTreePath   *path,
	GtkTreeIter   *iter,
	gpointer	*id )
{
	(void)path;
	gtk_tree_model_get (model, iter, ALBUMLIST_ID, id, -1);
}

static void browse_artist_select_on_change( 
	GtkTreeSelection *sel, 
	GtkTreeView *albview )
{
	gint id = -1;
	duc_it_album *it = NULL;

	gtk_tree_selection_selected_foreach( sel,
		(GtkTreeSelectionForeachFunc)browse_artist_each_getid, 
		(gpointer)&id);

	if( id >= 0 )
		it = duc_cmd_albumsartist( con, id );

	album_list_populate( GTK_TREE_VIEW(albview), it );
	duc_it_album_done( it );	
}

static void browse_album_select_on_change(
	GtkTreeSelection *sel, 
	GtkTreeView *trkview )
{
	gint id = -1;
	duc_it_track *it = NULL;

	gtk_tree_selection_selected_foreach( sel,
		(GtkTreeSelectionForeachFunc)browse_album_each_getid, 
		(gpointer)&id);

	if( id >= 0 )
		it = duc_cmd_tracksalbum( con, id );

	track_list_populate( trkview, it );
	duc_it_track_done( it );	
}


GtkWidget *browse_window( void )
{
	GtkWidget *me;
	GtkWidget *hpane, *vpane;
	GtkWidget *artwin, *albwin, *trkwin;
	GtkWidget *artview, *albview, *trkview;
	GtkTreeModel *artstore, *albstore, *trkstore;
	GtkTreeSelection *artsel, *albsel, *trksel;
	duc_it_artist *artists;

	hpane = gtk_hpaned_new();

	vpane = gtk_vpaned_new();
	gtk_paned_pack2( GTK_PANED(hpane), vpane, TRUE, TRUE );
	gtk_widget_show( vpane );

	/* artist list */

	artists = duc_cmd_artistlist( con );

	artwin = gtk_scrolled_window_new( GTK_ADJUSTMENT(NULL), GTK_ADJUSTMENT(NULL) );
	gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW(artwin), 
		GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
	gtk_paned_pack1( GTK_PANED(hpane), artwin, FALSE, TRUE );
	gtk_widget_show( artwin );

	artview = artist_list_new_with_list( artists );
	artsel = gtk_tree_view_get_selection( GTK_TREE_VIEW(artview));
	artstore = gtk_tree_view_get_model( GTK_TREE_VIEW(artview) );
	gtk_container_add( GTK_CONTAINER(artwin), artview );
	gtk_widget_show( artview );

	duc_it_artist_done( artists );

	/* TODO: update album store when selection changes */
	/* TODO: context menu? reread artist list? */

	/* album list */

	albwin = gtk_scrolled_window_new( GTK_ADJUSTMENT(NULL), GTK_ADJUSTMENT(NULL) );
	gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW(albwin), 
		GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
	gtk_paned_pack1( GTK_PANED(vpane), albwin, TRUE, TRUE);
	gtk_widget_show( albwin );

	albview = album_list_new(FALSE);
	albsel = gtk_tree_view_get_selection( GTK_TREE_VIEW(albview));
	albstore = gtk_tree_view_get_model( GTK_TREE_VIEW(albview) );
	gtk_container_add( GTK_CONTAINER(albwin), albview );
	gtk_widget_show( albview );

	/* TODO: update track store when selection changes */
	/* TODO: context menu? */
	/* TODO: un-/hide track artist column */


	/* track list */

	trkwin = gtk_scrolled_window_new( GTK_ADJUSTMENT(NULL), GTK_ADJUSTMENT(NULL) );
	gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW(trkwin), 
		GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
	gtk_paned_pack2( GTK_PANED(vpane), trkwin, TRUE, TRUE );
	gtk_widget_show( trkwin );

	trkview = track_list_new(FALSE);
	trksel = gtk_tree_view_get_selection( GTK_TREE_VIEW(trkview));
	gtk_tree_selection_set_mode( GTK_TREE_SELECTION(trksel), GTK_SELECTION_MULTIPLE );
	trkstore = gtk_tree_view_get_model( GTK_TREE_VIEW(trkview) );
	gtk_container_add( GTK_CONTAINER(trkwin), trkview );
	gtk_widget_show( trkview );


	/* interconnect parts */

	g_signal_connect( artsel, "changed", 
		G_CALLBACK(browse_artist_select_on_change), albview );
	g_signal_connect( albsel, "changed", 
		G_CALLBACK(browse_album_select_on_change), trkview );


	me = childwindow_new( "browse", hpane );

	gtk_window_set_default_size(GTK_WINDOW(me), 700, 400);
	gtk_paned_set_position( GTK_PANED(hpane), 300 );
	gtk_paned_set_position( GTK_PANED(vpane), 200 );

	return me;
}


