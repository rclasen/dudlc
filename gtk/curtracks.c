/*
 * Copyright (c) 2008 Rainer Clasen
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

#include "common.h"

static GtkWidget *tracks_view;

static void selection_on_change(
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
	(void)action;
	gtk_toggle_action_set_active(GTK_TOGGLE_ACTION(data), FALSE);
}

typedef struct {
	gint	artist;
	gint	album;
	gint	track;
} t_curids;

static void act_browse_each( 
	GtkTreeModel	*model,
	GtkTreePath	*path,
	GtkTreeIter	*iter,
	t_curids	*ids)
{
	(void)path;
	gtk_tree_model_get (model, iter, TRACKLIST_ARTIST_ID, &ids->artist, -1);
	gtk_tree_model_get (model, iter, TRACKLIST_ALBUM_ID, &ids->album, -1);
	gtk_tree_model_get (model, iter, TRACKLIST_ID, &ids->track, -1);
}

static void act_browse( GtkAction *action, GtkTreeView *view )
{
	GtkWidget *win;
	t_curids ids = {
		.artist	= -1,
		.album	= -1,
		.track	= -1,
	};
	GtkTreeSelection *sel;

	(void)action;

	sel = gtk_tree_view_get_selection( view );
	gtk_tree_selection_selected_foreach( sel, 
		(GtkTreeSelectionForeachFunc)act_browse_each, &ids );

	win = browse_window( ids.artist, ids.album, ids.track );
	gtk_widget_show( win );
}

static void act_queuealbum( GtkAction *action, gpointer *data )
{
	(void)action;
	track_list_select_queuealbum( GTK_TREE_VIEW(data) );
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

	{ "ShowMenu", NULL, "_Show", NULL, NULL, NULL },
	{ "QueueMenu", NULL, "_Queue", NULL, NULL, NULL },
};

static GtkActionEntry action_track_one[] = {
	{ "Browse", GTK_STOCK_OPEN, "B_rowse Track", "<control>R", 
		"show related information for selected track",
		G_CALLBACK(act_browse) },
	{ "QueueAlbum", GTK_STOCK_CDROM, "_Queue Album", NULL, 
		"queue selected album",
		G_CALLBACK(act_queuealbum) },
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

	"    <menu name='Show' action='ShowMenu'>"
	"      <menuitem name='Browse' action='Browse' />"
	"    </menu>"

	"    <menu name='Queue' action='QueueMenu'>"
	"      <menuitem name='Tracks' action='QueueTracks' />"
	"      <menuitem name='Album' action='QueueAlbum' />"
	"    </menu>"
	/* TODO: filter artst, album */
	/* TODO: hotkey to queue currently focused+selected album/track */
	/* TODO: show queue, search */

	"  </menubar>"	

/* toolbar */
	"  <toolbar name='Toolbar'>"

	"    <placeholder name='Show'>"
	"      <separator />"
	"      <toolitem name='Browse' action='Browse' />"
	"      <separator />"
	"    </placeholder>"

	"    <placeholder name='Queue'>"
	"      <separator />"
	"      <toolitem name='Tracks' action='QueueTracks' />"
	"      <toolitem name='Album' action='QueueAlbum' />"
	"      <separator />"
	"    </placeholder>"

	"  </toolbar>"

/* treeview context menus */
	"  <popup name='TrackPopup'>"
	"    <menuitem name='Browse' action='Browse' />"
	"    <separator />"
	"    <menuitem name='Tracks' action='QueueTracks' />"
	"    <menuitem name='Album' action='QueueAlbum' />"
	"  </popup>"

	"</ui>";


GtkWidget *curtracks_new( GtkAction *action )
{
	GtkWidget *win;
	GtkTreeSelection *sel;
	GtkActionGroup *ag_always, *ag_track_one, *ag_track_any;
	GtkUIManager *ui;
	GtkWidget *context;

	tracks_view = track_list_new(TRUE);
	sel = gtk_tree_view_get_selection( GTK_TREE_VIEW(tracks_view));
	gtk_tree_selection_set_mode( GTK_TREE_SELECTION(sel), GTK_SELECTION_MULTIPLE );
	g_signal_connect( sel, "changed", 
		G_CALLBACK(selection_on_change), NULL );

	/* actions + uimanager for menu, toolbar + hotkeys */
	ag_always = gtk_action_group_new("TrackAlways");
        gtk_action_group_add_actions(ag_always, action_always, 
		G_N_ELEMENTS(action_always), action );

	ag_track_one = gtk_action_group_new("TrackOne");
	g_object_set_data( G_OBJECT(sel), "agroup-one", ag_track_one);
	gtk_action_group_set_sensitive(ag_track_one, FALSE );
        gtk_action_group_add_actions(ag_track_one, action_track_one, 
		G_N_ELEMENTS(action_track_one), tracks_view );

	ag_track_any = gtk_action_group_new("TrackAny");
	g_object_set_data( G_OBJECT(sel), "agroup-any", ag_track_any);
	gtk_action_group_set_sensitive(ag_track_any, FALSE );
        gtk_action_group_add_actions(ag_track_any, action_track_any, 
		G_N_ELEMENTS(action_track_any), tracks_view );

	ui = gtk_ui_manager_new();
	gtk_ui_manager_insert_action_group(ui, ag_always, 0);
	gtk_ui_manager_insert_action_group(ui, ag_track_one, 1);
	gtk_ui_manager_insert_action_group(ui, ag_track_any, 2);

	/* top window */
	win = childscroll_new( "tracks of current album", tracks_view, ui, uidesc );
	gtk_signal_connect( GTK_OBJECT(win), "delete_event", 
			G_CALLBACK( toggle_win_on_delete ), action);
	gtk_window_set_default_size(GTK_WINDOW(win), 500, 300);

	/* context menu */
	context = gtk_ui_manager_get_widget( ui, "/TrackPopup" );
	context_add( GTK_TREE_VIEW(tracks_view), GTK_MENU(context) );

	/* free ui_manager */
	g_object_unref( G_OBJECT(ui) );

	return win;
}

void curtracks_populate( duc_it_track *it )
{
	track_list_populate( GTK_TREE_VIEW(tracks_view), it );
}

void curtracks_clear( void )
{
	track_list_clear( GTK_TREE_VIEW(tracks_view) );
}


