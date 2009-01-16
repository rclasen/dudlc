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
	GtkActionGroup *ag;
	gint id = -1;
	duc_it_album *it = NULL;

	gtk_tree_selection_selected_foreach( sel,
		(GtkTreeSelectionForeachFunc)browse_artist_each_getid, 
		(gpointer)&id);

	if( id >= 0 )
		it = duc_cmd_albumsartist( con, id );

	album_list_populate( GTK_TREE_VIEW(albview), it );
	duc_it_album_done( it );	

	if( NULL != (ag = g_object_get_data( G_OBJECT(sel), "agroup-one" )))
		gtk_action_group_set_sensitive(ag, id >= 0 );
}

static void browse_album_select_on_change(
	GtkTreeSelection *sel, 
	GtkTreeView *trkview )
{
	GtkActionGroup *ag;
	gint id = -1;
	duc_it_track *it = NULL;

	gtk_tree_selection_selected_foreach( sel,
		(GtkTreeSelectionForeachFunc)browse_album_each_getid, 
		(gpointer)&id);

	if( id >= 0 )
		it = duc_cmd_tracksalbum( con, id );

	track_list_populate( trkview, it );
	duc_it_track_done( it );	

	if( NULL != (ag = g_object_get_data( G_OBJECT(sel), "agroup-one" )))
		gtk_action_group_set_sensitive(ag, id >= 0 );
}

static void browse_track_select_on_change(
	GtkTreeSelection *sel, 
	gpointer data )
{
	GtkActionGroup *ag;
	gint selected;

	(void)data;

	selected = tree_select_count( sel );

	if( NULL != (ag = g_object_get_data(G_OBJECT(sel),"agroup-one")))
		gtk_action_group_set_sensitive(ag, selected == 1);
	
	if( NULL != (ag = g_object_get_data(G_OBJECT(sel),"agroup-any")))
		gtk_action_group_set_sensitive(ag, selected >= 1);
}

static void act_close( GtkAction *action, gpointer *data )
{
	GtkWidget *win;

	(void)data;

	win = g_object_get_data( G_OBJECT(action), "topwindow" );
	gtk_widget_destroy(win);
}

static void act_queuealbum( GtkAction *action, gpointer *data )
{
	(void)action;
	album_list_select_queuealbum( GTK_TREE_VIEW(data) );
}

static void act_queuetrack( GtkAction *action, gpointer *data )
{
	(void)action;
	track_list_select_queueadd( GTK_TREE_VIEW(data) );
}


static GtkActionEntry action_always[] = {
	{ "FileMenu", NULL, "_File", NULL, NULL, NULL },
	{ "Close", GTK_STOCK_CLOSE, "_Close", "<control>W", 
		"close this window", G_CALLBACK(act_close) },

	{ "QueueMenu", NULL, "_Queue", NULL, NULL, NULL },
};

static GtkActionEntry action_artist[] = {
};

static GtkActionEntry action_album[] = {
	{ "QueueAlbum", GTK_STOCK_CDROM, "_Queue Album", NULL, 
		"queue selected album",
		G_CALLBACK(act_queuealbum) },
};

static GtkActionEntry action_track_one[] = {
};

static GtkActionEntry action_track_any[] = {
	{ "QueueTracks", GTK_STOCK_ADD, "_Queue Tracks", NULL, 
		"queue selected tracks",
		G_CALLBACK(act_queuetrack) },
};

static const char *uidesc =
	"<ui>"
/* menu bar */
	"  <menubar name='Menu'>"

	"    <menu name='File' action='FileMenu'>"
	"      <menuitem name='Close' action='Close' />"
	"    </menu>"

	"    <menu name='Queue' action='QueueMenu'>"
	"      <menuitem name='Tracks' action='QueueTracks' />"
	"      <menuitem name='Album' action='QueueAlbum' />"
	"    </menu>"
	/* TODO: filter artst, album */
	/* TODO: refresh artist list */
	/* TODO: un-/hide track artist column */
	/* TODO: show non-album tracks of artist */
	/* TODO: hotkey to queue currently focused+selected album/track */
	/* TODO: show queue, search */

	"  </menubar>"	

/* toolbar */
	"  <toolbar name='Toolbar'>"

	"    <placeholder name='Queue'>"
	"      <separator />"
	"      <toolitem name='Tracks' action='QueueTracks' />"
	"      <toolitem name='Album' action='QueueAlbum' />"
	"      <separator />"
	"    </placeholder>"

	"  </toolbar>"

/* treeview context menus */
	"  <popup name='ArtistPopup'>"
	"  </popup>"

	"  <popup name='AlbumPopup'>"
	"    <menuitem name='Album' action='QueueAlbum' />"
	"  </popup>"

	"  <popup name='TrackPopup'>"
	"    <menuitem name='Tracks' action='QueueTracks' />"
	"  </popup>"

	"</ui>";


GtkWidget *browse_window( gint artist, gint album, gint track )
{
	GtkWidget *me;
	GtkWidget *hpane, *vpane;
	GtkWidget *artwin, *albwin, *trkwin;
	GtkWidget *artview, *albview, *trkview;
	GtkTreeModel *artstore, *albstore, *trkstore;
	GtkTreeSelection *artsel, *albsel, *trksel;
	duc_it_artist *artists;
	GtkActionGroup *ag_always, *ag_artist, *ag_album, *ag_track_one, *ag_track_any;
	GtkAction *close;
	GtkUIManager *ui;
	GtkWidget *context;

	/* top window layout */

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
	g_signal_connect( trksel, "changed", 
		G_CALLBACK(browse_track_select_on_change), NULL );


	/* actions + uimanager for menu, toolbar + hotkeys */
	ag_always = gtk_action_group_new("BrowseAlways");
        gtk_action_group_add_actions(ag_always, action_always, 
		G_N_ELEMENTS(action_always), NULL );
	close = gtk_action_group_get_action( ag_always, "Close" );

	ag_artist = gtk_action_group_new("BrowseArtist");
	g_object_set_data( G_OBJECT(artsel), "agroup-one", ag_artist);
	gtk_action_group_set_sensitive(ag_artist, FALSE );
        gtk_action_group_add_actions(ag_artist, action_artist, 
		G_N_ELEMENTS(action_artist), artview );

	ag_album = gtk_action_group_new("BrowseAlbum");
	g_object_set_data( G_OBJECT(albsel), "agroup-one", ag_album);
	gtk_action_group_set_sensitive(ag_album, FALSE );
        gtk_action_group_add_actions(ag_album, action_album, 
		G_N_ELEMENTS(action_album), albview );

	ag_track_one = gtk_action_group_new("BrowseTrackOne");
	g_object_set_data( G_OBJECT(trksel), "agroup-one", ag_track_one);
	gtk_action_group_set_sensitive(ag_track_one, FALSE );
        gtk_action_group_add_actions(ag_track_one, action_track_one, 
		G_N_ELEMENTS(action_track_one), trkview );

	ag_track_any = gtk_action_group_new("BrowseTrackAny");
	g_object_set_data( G_OBJECT(trksel), "agroup-any", ag_track_any);
	gtk_action_group_set_sensitive(ag_track_any, FALSE );
        gtk_action_group_add_actions(ag_track_any, action_track_any, 
		G_N_ELEMENTS(action_track_any), trkview );

	ui = gtk_ui_manager_new();
	gtk_ui_manager_insert_action_group(ui, ag_always, 0);
	gtk_ui_manager_insert_action_group(ui, ag_artist, 1);
	gtk_ui_manager_insert_action_group(ui, ag_album, 2);
	gtk_ui_manager_insert_action_group(ui, ag_track_one, 3);
	gtk_ui_manager_insert_action_group(ui, ag_track_any, 4);


	/* top window */

	me = childwindow_new( "browse", hpane, ui, uidesc );
	gtk_window_set_default_size(GTK_WINDOW(me), 800, 400);
	gtk_paned_set_position( GTK_PANED(hpane), 300 );
	gtk_paned_set_position( GTK_PANED(vpane), 200 );

	g_object_set_data( G_OBJECT(close), "topwindow", me );


	/* context menus */

	context = gtk_ui_manager_get_widget( ui, "/ArtistPopup" );
	context_add( GTK_TREE_VIEW(artview), GTK_MENU(context) );

	context = gtk_ui_manager_get_widget( ui, "/AlbumPopup" );
	context_add( GTK_TREE_VIEW(albview), GTK_MENU(context) );

	context = gtk_ui_manager_get_widget( ui, "/TrackPopup" );
	context_add( GTK_TREE_VIEW(trkview), GTK_MENU(context) );


	/* select track */

	if( artist >= 0 ){
		artist_list_select( GTK_TREE_VIEW(artview), artist );
		if( album >= 0 ){
			album_list_select( GTK_TREE_VIEW(albview), album );
			if( track >= 0 )
				track_list_select( GTK_TREE_VIEW(trkview), track );
		}
	}


	/* free ui_manager */
	g_object_unref( G_OBJECT(ui) );

	return me;
}


