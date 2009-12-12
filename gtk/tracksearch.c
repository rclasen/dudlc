/*
 * Copyright (c) 2008 Rainer Clasen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

#include <string.h>

#include "common.h"

typedef struct {
	GtkWidget	*input;
	GtkWidget	*list;
	GtkWidget	*status;
	gchar		*filter;
} search_widgets;

static void search_button_on_click( GtkButton *widget, search_widgets *wg )
{
	duc_it_track *it;
	(void)widget;

	gtk_statusbar_pop( GTK_STATUSBAR(wg->status), 0 );
	g_free(wg->filter);

	wg->filter = g_strdup( gtk_entry_get_text( GTK_ENTRY(wg->input) ));

	gtk_statusbar_push( GTK_STATUSBAR(wg->status), 0,
		"searching ..." );
	// TODO: g_locale_to_utf8( wg->status, len, r, w, &err )
	it = duc_cmd_tracksearchf( con, wg->filter );
	gtk_statusbar_pop( GTK_STATUSBAR(wg->status), 0 );

	gtk_statusbar_push( GTK_STATUSBAR(wg->status), 0, (it == NULL)
		? "failed" : wg->filter );
	track_list_populate( GTK_TREE_VIEW(wg->list), it );
}

static void tracksearch_on_destroy( GtkWidget *widget, search_widgets *wg )
{
	(void)widget;
	g_free(wg->filter);
	g_free(wg);
}

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
	GtkWidget *win;

	(void)data;

	win = g_object_get_data( G_OBJECT(action), "topwindow" );
	gtk_widget_destroy(win);
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
	/* TODO: show queue */

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


GtkWidget *tracksearch_window( void )
{
	GtkWidget *win;
	GtkWidget *winbox;
	GtkWidget *search_box;
	GtkWidget *search_label;
	GtkWidget *search_button;
	GtkWidget *scroll;
	GtkTreeSelection *sel;
	search_widgets *wg;
	GtkActionGroup *ag_always, *ag_track_one, *ag_track_any;
	GtkAction *close;
	GtkUIManager *ui;
	GtkWidget *context;


	/* TODO: use g_object_set_data() */
	wg = g_malloc(sizeof(search_widgets));
	memset(wg, 0, sizeof(search_widgets));


	/* top box */
	winbox = gtk_vbox_new( FALSE, 0 );



	/* input */
	search_box = gtk_hbox_new( FALSE, 0 );
	gtk_box_pack_start( GTK_BOX(winbox), search_box, FALSE, FALSE, 1 );
	gtk_widget_show( search_box );

	search_label = gtk_label_new("filter:");
	gtk_box_pack_start( GTK_BOX(search_box), search_label, FALSE, FALSE, 1 );
	gtk_widget_show( search_label );

	/* TODO: give some hint about filter syntax */
	/* TODO: input history for filter */
	/* TODO: auto complete for filter */
	wg->input = gtk_entry_new();
	g_object_set( wg->input, "activates-default", TRUE, NULL );
	gtk_box_pack_start( GTK_BOX(search_box), wg->input, TRUE, TRUE, 1 );
	gtk_widget_show( wg->input );

	search_button = gtk_button_new_with_label("search");
	gtk_signal_connect( GTK_OBJECT(search_button), "clicked",
		G_CALLBACK(search_button_on_click), wg );
	gtk_box_pack_start( GTK_BOX(search_box), search_button, FALSE, FALSE, 1 );
	GTK_WIDGET_SET_FLAGS( search_button, GTK_CAN_DEFAULT);

	gtk_widget_show( search_button );



	/* scrolled result list */
	scroll = gtk_scrolled_window_new( GTK_ADJUSTMENT(NULL), GTK_ADJUSTMENT(NULL) );
	gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW(scroll),
		GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );

	wg->list = track_list_new_with_list( TRUE, NULL );
	sel = gtk_tree_view_get_selection( GTK_TREE_VIEW(wg->list) );
	gtk_tree_selection_set_mode( GTK_TREE_SELECTION(sel), GTK_SELECTION_MULTIPLE );
	g_signal_connect( G_OBJECT(sel), "changed",
		G_CALLBACK(selection_on_change), NULL );
	gtk_container_add( GTK_CONTAINER(scroll), wg->list );
	gtk_widget_show( wg->list );

	gtk_box_pack_start( GTK_BOX( winbox ), scroll, TRUE, TRUE, 0 );
	gtk_widget_show( scroll );


	/* status bar */
	wg->status = gtk_statusbar_new();
	gtk_box_pack_start( GTK_BOX(winbox), wg->status, FALSE, FALSE, 0 );
	gtk_widget_show( wg->status );


	/* actions + uimanager for menu, toolbar + hotkeys */
	ag_always = gtk_action_group_new("TracksearchAlways");
        gtk_action_group_add_actions(ag_always, action_always,
		G_N_ELEMENTS(action_always), NULL );
	close = gtk_action_group_get_action( ag_always, "Close" );

	ag_track_one = gtk_action_group_new("TracksearchOne");
	g_object_set_data( G_OBJECT(sel), "agroup-one", ag_track_one);
	gtk_action_group_set_sensitive(ag_track_one, FALSE );
        gtk_action_group_add_actions(ag_track_one, action_track_one,
		G_N_ELEMENTS(action_track_one), wg->list );

	ag_track_any = gtk_action_group_new("TracksearchAny");
	g_object_set_data( G_OBJECT(sel), "agroup-any", ag_track_any);
	gtk_action_group_set_sensitive(ag_track_any, FALSE );
        gtk_action_group_add_actions(ag_track_any, action_track_any,
		G_N_ELEMENTS(action_track_any), wg->list );

	ui = gtk_ui_manager_new();
	gtk_ui_manager_insert_action_group(ui, ag_always, 0);
	gtk_ui_manager_insert_action_group(ui, ag_track_one, 1);
	gtk_ui_manager_insert_action_group(ui, ag_track_any, 2);

	/* top window */
	win = childwindow_new( "search tracks", winbox, ui, uidesc );
	gtk_window_set_default_size(GTK_WINDOW(win), 600, 300);
	g_object_set_data( G_OBJECT(close), "topwindow", win );
	gtk_window_set_default( GTK_WINDOW(win), search_button );

	gtk_signal_connect( GTK_OBJECT(win), "destroy",
		G_CALLBACK(tracksearch_on_destroy), wg);


	/* context menu */
	context = gtk_ui_manager_get_widget( ui, "/TrackPopup" );
	context_add( GTK_TREE_VIEW(wg->list), GTK_MENU(context) );

	/* free ui_manager */
	g_object_unref( G_OBJECT(ui) );


	return win;
}

