/*
 * Copyright (c) 2008 Rainer Clasen
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

#include "common.h"

static GtkWidget *queue_view = NULL;
static GtkTreeModel *queue_store = NULL;

typedef struct {
	gint	artist;
	gint	album;
	gint	track;
} t_curids;

static void act_close( GtkAction *action, gpointer *data )
{
	(void)action;
	gtk_toggle_action_set_active(GTK_TOGGLE_ACTION(data), FALSE);
}

static void act_clear( GtkAction *action, gpointer *data )
{
	(void)action;
	(void)data;
	duc_cmd_queueclear( con );
}

static void act_unqueue_tracks( GtkAction *action, GtkTreeView *view )
{
	(void)action;
	queue_list_select_deltrack( view );
}

static void act_browse_each( 
	GtkTreeModel	*model,
	GtkTreePath	*path,
	GtkTreeIter	*iter,
	t_curids	*ids)
{
	(void)path;
	gtk_tree_model_get (model, iter, QUEUELIST_ARTIST_ID, &ids->artist, -1);
	gtk_tree_model_get (model, iter, QUEUELIST_ALBUM_ID, &ids->album, -1);
	gtk_tree_model_get (model, iter, QUEUELIST_TRACK_ID, &ids->track, -1);
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

static void selection_on_change( GtkTreeSelection *sel, gpointer data )
{
	gint selected;
	GtkActionGroup *ag;

	(void)data;

	selected = tree_select_count( sel );

	if( NULL != (ag = g_object_get_data(G_OBJECT(sel),"agroup-one")))
		gtk_action_group_set_sensitive(ag, selected == 1);
	
	if( NULL != (ag = g_object_get_data(G_OBJECT(sel),"agroup-any")))
		gtk_action_group_set_sensitive(ag, selected >= 1);
	
}

static GtkActionEntry action_always[] = {
	{ "FileMenu", NULL, "_File", NULL, NULL, NULL },
	{ "Close", GTK_STOCK_CLOSE, "_Close", "<control>W", 
		"close this window", G_CALLBACK(act_close) },

	{ "EditMenu", NULL, "_Edit", NULL, NULL, NULL },
	{ "Clear", GTK_STOCK_CLEAR, "_Clear Queue", NULL,
		"clear queue", G_CALLBACK(act_clear) },

	{ "ShowMenu", NULL, "_Show", NULL, NULL, NULL },
};

static GtkActionEntry action_any[] = {
	{ "UnqueueTracks", GTK_STOCK_DELETE, "Unqueue Tracks", NULL, 
		"unqueue selected tracks", 
		G_CALLBACK(act_unqueue_tracks) },
};

static GtkActionEntry action_one[] = {
	{ "Browse", GTK_STOCK_OPEN, "B_rowse Track", "<control>R", 
		"show related information for selected track",
		G_CALLBACK(act_browse) },
	/* TODO: Unqueue album, artist, user */
	/* TODO: filter artist, album  */
	/* TODO: show search */
};

static const char *uidesc =
	"<ui>"
/* menu bar */
	"  <menubar name='Menu'>"

	"    <menu name='File' action='FileMenu'>"
	"      <menuitem name='Close' action='Close' />"
	"    </menu>"

	"    <menu name='Edit' action='EditMenu'>"
	"      <menuitem name='Clear' action='Clear' />"
	"      <menuitem name='UnqueueTracks' action='UnqueueTracks' />"
	"    </menu>"

	"    <menu name='Show' action='ShowMenu'>"
	"      <menuitem name='Browse' action='Browse' />"
	"    </menu>"

	"  </menubar>"	

/* toolbar */
	"  <toolbar name='Toolbar'>"

	"    <placeholder name='Unqueue'>"
	"      <separator />"
	"      <toolitem name='Clear' action='Clear' />"
	"      <toolitem name='UnqueueTracks' action='UnqueueTracks' />"
	"      <separator />"
	"    </placeholder>"

	"    <placeholder name='Show'>"
	"      <separator />"
	"      <toolitem name='Browse' action ='Browse' />"
	"      <separator />"
	"    </placeholder>"

	"  </toolbar>"

/* treeview context menu */
	"  <popup name='ViewPopup'>"
	"    <menuitem name='UnqueueTracks' action='UnqueueTracks' />"
	"    <separator />"
	"    <menuitem name='Browse' action ='Browse' />"
	"  </popup>"
	"</ui>";


GtkWidget *queuewin_new( GtkAction *action )
{
	GtkWidget *win;
	GtkTreeSelection *sel;
	GtkActionGroup *ag_always, *ag_any, *ag_one;
	GtkUIManager *ui;
	GtkWidget *context;

	queue_view = queue_list_new();
	queue_store = gtk_tree_view_get_model( GTK_TREE_VIEW(queue_view));

	sel = gtk_tree_view_get_selection( GTK_TREE_VIEW(queue_view));
	gtk_tree_selection_set_mode( GTK_TREE_SELECTION(sel), GTK_SELECTION_MULTIPLE );
	g_signal_connect( G_OBJECT(sel), "changed", 
		G_CALLBACK(selection_on_change), NULL );

	/* actions + uimanager for menu, toolbar + hotkeys */
	ag_always = gtk_action_group_new("QueueAlways");
        gtk_action_group_add_actions(ag_always, action_always, 
		G_N_ELEMENTS(action_always), action );

	ag_one = gtk_action_group_new("QueueOne");
	g_object_set_data( G_OBJECT(sel), "agroup-one", ag_one);
	gtk_action_group_set_sensitive(ag_one, FALSE );
        gtk_action_group_add_actions(ag_one, action_one, 
		G_N_ELEMENTS(action_one), queue_view );

	ag_any = gtk_action_group_new("QueueAny");
	g_object_set_data( G_OBJECT(sel), "agroup-any", ag_any);
	gtk_action_group_set_sensitive(ag_any, FALSE );
        gtk_action_group_add_actions(ag_any, action_any, 
		G_N_ELEMENTS(action_any), queue_view );

	ui = gtk_ui_manager_new();
	gtk_ui_manager_insert_action_group(ui, ag_always, 0);
	gtk_ui_manager_insert_action_group(ui, ag_any, 1);
	gtk_ui_manager_insert_action_group(ui, ag_one, 2);

 	win = childscroll_new( "queue", queue_view, ui, uidesc );
	gtk_signal_connect( GTK_OBJECT(win), "delete_event", 
			G_CALLBACK( toggle_win_on_delete ), action);
	gtk_window_set_default_size(GTK_WINDOW(win), 800, 300);

	/* connect context menu */
	context = gtk_ui_manager_get_widget( ui, "/ViewPopup" );
	context_add( GTK_TREE_VIEW(queue_view), GTK_MENU(context) );

	/* free ui_manager */
	g_object_unref( G_OBJECT(ui) );

	return win;
}

void queuewin_populate( duc_it_queue *queue )
{
	queue_list_populate( GTK_TREE_VIEW(queue_view), queue );
}

void queuewin_clear( void )
{
	queue_list_clear( GTK_TREE_VIEW(queue_view) );
}

void queuewin_add( duc_queue *item )
{
	queue_list_store_add( queue_store, item );
}

void queuewin_del( int id )
{
	queue_list_store_del( queue_store, id );
}




