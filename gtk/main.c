/*
 * Copyright (c) 2008 Rainer Clasen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <dudlc.h>

#include "common.h"

dudlc *con = NULL;
static GtkWidget *main_win, *main_menu;
static int needupdate = 0;

static GtkWidget *queue_win;
static GtkWidget *albums_win;
static GtkWidget *tracks_win;

/************************************************************
 * dudl interaction
 */

static gboolean detail_update_idle( gpointer data )
{
	duc_track *cur;
	duc_it_track *tracks;
	duc_it_album *albums;

	(void)data;

	if( !needupdate )
		return FALSE;

	if( NULL == (cur = duc_cmd_curtrack( con )) )
		return FALSE;

	if( NULL != (tracks = duc_cmd_tracksalbum( con, cur->album->id ))){
		curtracks_populate( tracks );
		duc_it_track_done( tracks );
	}

	if( NULL != (albums = duc_cmd_albumsartist( con, cur->artist->id ))){
		curalbums_populate( albums );
		duc_it_album_done( albums );
	}

	duc_track_free(cur);
	needupdate=0;

	return FALSE;
}

static void detail_update( duc_track *t )
{
	playbox_detail_update( t );
	needupdate++;
	g_idle_add( detail_update_idle, NULL );
}

static void detail_clear( void )
{
	playbox_detail_clear();
	curalbums_clear();
	curtracks_clear();
}

static gboolean iowatch_dudl( GIOChannel *source, GIOCondition cond, gpointer data)
{
	(void)source;
	(void)cond;
	duc_poll( (dudlc*)data );
	return TRUE;
}

static void ev_disc( dudlc *c )
{
	while( g_source_remove_by_user_data( c ) );
	playbox_buttons_enable( pl_offline );
	detail_clear();
	queuewin_clear();
}

static void ev_conn( dudlc *c )
{
	GIOChannel *cchan;
	duc_playstatus stat;
	duc_track *t;
	duc_it_queue *q;

	cchan = g_io_channel_unix_new(duc_fd(c));
	g_io_add_watch(cchan, G_IO_IN | G_IO_HUP | G_IO_ERR, iowatch_dudl, c );

	stat = duc_cmd_status( c );
	playbox_buttons_enable( stat );

	if( NULL != (t = duc_cmd_curtrack( c ))){
		detail_update( t );

		duc_track_free( t );
	}

	if( NULL != (q = duc_cmd_queuelist(c))){
		queuewin_populate( q );
		duc_it_queue_done( q );
	}

}

static void ev_nexttrack( dudlc *c, duc_track *t )
{
	(void)c;
	playbox_buttons_enable( pl_play );
	detail_update( t );
}

static void ev_stopped( dudlc *c )
{
	(void)c;
	playbox_buttons_enable( pl_stop );
	detail_clear();
}

static void ev_paused( dudlc *c )
{
	(void)c;
	playbox_buttons_enable( pl_pause );
}

static void ev_resumed( dudlc *c )
{
	(void)c;
	playbox_buttons_enable( pl_play );
}

static void ev_elapsed( dudlc *c, int r )
{
	(void)c;
	playbox_progress_update( r );
}

static void ev_queuefetch( dudlc *c, duc_queue *queue )
{
	(void)c;
	queuewin_del( queue->id );
}

static void ev_queueadd( dudlc *c, duc_queue *queue )
{
	(void)c;
	queuewin_add( queue );
}

static void ev_queuedel( dudlc *c, duc_queue *queue )
{
	(void)c;
	queuewin_del( queue->id );
}

static void ev_queueclear( dudlc *c )
{
	(void)c;
	queuewin_clear();
}

/************************************************************
 * widget signal handler
 */

static void shrink_y( GtkWindow *win )
{
	gint x;
	gtk_window_get_size( win, &x, NULL );
	gtk_window_resize( win, x, 1 );
}

static void menu_toggle( GtkAction *action, gpointer data )
{
	(void)data;
	if( gtk_toggle_action_get_active( GTK_TOGGLE_ACTION(action)))
		gtk_widget_show(GTK_WIDGET(main_menu));
	else {
		gtk_widget_hide(GTK_WIDGET(main_menu));
		shrink_y(GTK_WINDOW(main_win));
	}
}

static void buttons_toggle( GtkAction *action, gpointer data )
{
	gboolean show;
	(void)data;

	show = gtk_toggle_action_get_active(GTK_TOGGLE_ACTION(action));

	playbox_buttons_show( show );
	if(! show )
		shrink_y(GTK_WINDOW(main_win));
}

static void toggle_win_on_toggle( GtkAction *action, GtkWidget *win )
{
	if( gtk_toggle_action_get_active( GTK_TOGGLE_ACTION(action)))
		gtk_widget_show(GTK_WIDGET(win));
	else {
		gtk_widget_hide(GTK_WIDGET(win));
	}
}

static void queue_toggle( GtkAction *action, gpointer data )
{
	(void)data;
	toggle_win_on_toggle( action, queue_win );
}

static void tracks_toggle( GtkAction *action, gpointer data )
{
	(void)data;
	toggle_win_on_toggle( action, tracks_win );
}

static void albums_toggle( GtkAction *action, gpointer data )
{
	(void)data;
	toggle_win_on_toggle( action, albums_win );
}

static void open_browse( GtkAction *action, gpointer data )
{
	duc_track *track;
	GtkWidget *win;

	(void)action;
	(void)data;

	track = duc_cmd_curtrack( con );
	win = browse_window( track->album->artist->id,
		track->album->id, track->id );
	gtk_widget_show( win );
	duc_track_free( track );
}

static void open_tracksearch( GtkAction *action, gpointer data )
{
	GtkWidget *win;

	(void)action;
	(void)data;

	win = tracksearch_window();
	gtk_widget_show( win );
}

static void main_win_on_destroy( GtkWidget *widget, gpointer data )
{
	(void)widget;
	(void)data;
	gtk_main_quit();
}

int main( int argc, char **argv )
{
	options opt;
	char *opt_cfg = NULL;
	char *opt_geometry = NULL;
	gboolean opt_hmenu = FALSE;
	gboolean opt_hbuttons = FALSE;
	duc_events events;
	GtkWidget *mainbox;
	GtkWidget *playbox;
	GError *err = NULL;
	GOptionEntry option_entries[] = {
		{ "host",	'H', 0,
			G_OPTION_ARG_STRING,	&opt.host,
			"dudl server name to connect to",
			"hostname" },
		{ "port",	'p', 0,
			G_OPTION_ARG_INT,	&opt.port,
			"TCP port on dudl server to connect to",
			"port" },
		{ "user",	'u', 0,
			G_OPTION_ARG_STRING,	&opt.user,
			"dudl user name",
			"username" },
		{ "pass",	'P', 0,
			G_OPTION_ARG_STRING,	&opt.pass,
			"dudl password",
			"password" },
		{ "config",	'f', 0,
			G_OPTION_ARG_FILENAME,	&opt_cfg,
			"alternate config file",
			"config" },
		{ "geometry",	'g', 0,
			G_OPTION_ARG_STRING,	&opt_geometry,
			"initial window position and size",
			"geometry" },
		{ "hidemenu",	'm', 0,
			G_OPTION_ARG_NONE,	&opt_hmenu,
			"hide menu on startup", NULL },
		{ "hidebuttons",	'b', 0,
			G_OPTION_ARG_NONE,	&opt_hbuttons,
			"hide buttons on startup", NULL },
		{ NULL, 0, 0, 0, NULL, NULL, NULL }
	};
	GtkActionEntry action_entries[] = {
		{ "FileMenu", NULL, "_File", NULL, NULL, NULL },
		{ "Quit", GTK_STOCK_QUIT, "_Quit", "<control>Q",
			"Quit the program", G_CALLBACK(gtk_main_quit) },

		{ "OptionMenu", NULL, "_Options", NULL, NULL, NULL },

		{ "ShowMenu", NULL, "_Show", NULL, NULL, NULL },
		{ "SearchTrack", GTK_STOCK_FIND, "_Search Track", "<control>S",
			"search tracks", G_CALLBACK(open_tracksearch) },
		{ "Browse", GTK_STOCK_OPEN, "B_rowse Archive", "<control>R",
			"browse Archive", G_CALLBACK(open_browse) },
	};
	GtkToggleActionEntry action_tentries[] = {
		{ "OptButtons", NULL, "show _Buttons", "<control>B",
			"show/hide player buttons", G_CALLBACK(buttons_toggle), 1 },
		{ "OptMenu", NULL, "show _Menu", "<control>M",
			"show/hide menu", G_CALLBACK(menu_toggle), 1 },
		{ "OptQueue", NULL, "show Q_ueue", "<control>U",
			"show queue contents", G_CALLBACK(queue_toggle), 0},
		{ "OptArtist", NULL, "show _Artist", "<control>A",
			"show ablums of current artist",
			G_CALLBACK(albums_toggle), 0 },
		{ "OptAlbum", NULL, "show A_lbum", "<control>L",
			"show tracks of current album",
			G_CALLBACK(tracks_toggle), 0 },
	};
	const char * ui_description =
		"<ui>"
		"  <menubar name='MainMenu'>"
		"    <menu name='File' action='FileMenu'>"
		"      <menuitem name='Quit' action='Quit'/>"
		"    </menu>"
		"    <menu name='Show' action='ShowMenu'>"
		"      <menuitem name='Track' action='SearchTrack'/>"
		"      <menuitem name='Browse' action='Browse'/>"
		"    </menu>"
		"    <menu name='Option' action='OptionMenu'>"
		"      <menuitem name='Menu' action='OptMenu'/>"
		"      <menuitem name='Buttons' action='OptButtons'/>"
		"      <menuitem name='Queue' action='OptQueue'/>"
		"      <menuitem name='Album' action='OptAlbum'/>"
		"      <menuitem name='Artist' action='OptArtist'/>"
		"    </menu>"
		"  </menubar>"
		"</ui>";
        GtkActionGroup *agroup_main;
	GtkAccelGroup *accels;
	GtkUIManager *ui_manager;
	GtkAction *action;


	/* TODO: remove duplicate code for ui_managers */
	/* *TODO: error handling */
	/* TODO: remember window positions */
	/* TODO: playbox keyboard shortcuts */
	/* TODO: icons
	GTK_STOCK_CDROM queue_album
	GTK_STOCK_ADD + queue_track
	GTK_STOCK_REMOVE -

	GTK_STOCK_CANCEL
	GTK_STOCK_CLEAR
	GTK_STOCK_DELETE queue:del
	GTK_STOCK_STOP

	GTK_STOCK_FIND tracksearch
	GTK_STOCK_INDEX
	GTK_STOCK_INFO
	GTK_STOCK_OPEN browse
	GTK_STOCK_REFRESH browse: reload artist list
	 */
	/* TODO: check window/icon names */
	/* TODO: view/edit frontend for user, clients, tags, filtertop,
	 * history, album_search, artist_search,
	 */
	/* TODO: playbox context menu */
	/* TODO: retry connect every x seconds */
	/* TODO: drag'n'drop within queue to reorder */
	/* TODO: drag'n'drop towards queue */


	options_init( &opt );

	if( ! gtk_init_with_args( &argc, &argv, "dudl jukebox client for X11",
			option_entries, NULL, &err )){

		if( ! err )
			g_printerr( "init failed\n" );
		else if( err->domain == G_OPTION_ERROR )
			g_printerr( "%s\nuse --help for usage info\n", err->message);
		else
			g_printerr( "init failed: %s\n", err->message );
		g_clear_error( &err );
		exit(1);
	}

	if( ! opt_cfg )
		opt_cfg = g_strdup_printf( "%s/.dudlc.conf",
				g_get_home_dir() );

	options_read( &opt, opt_cfg );

	/* dudl part one */
	con = duc_new( opt.host, opt.port );
	duc_setauth( con, opt.user, opt.pass );


	/* main window */
	main_win = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	gtk_window_set_title( GTK_WINDOW(main_win), "dudlc" );
	gtk_signal_connect( GTK_OBJECT( main_win ), "destroy",
			G_CALLBACK( main_win_on_destroy ), NULL);

	mainbox = gtk_vbox_new( FALSE, 0 );
	gtk_container_add( GTK_CONTAINER( main_win ), mainbox );
	gtk_widget_show( mainbox );

	/* actions + uimanager for menu, toolbar + hotkeys */
	agroup_main = gtk_action_group_new("MainWindowActions");
        gtk_action_group_add_actions(agroup_main, action_entries,
		G_N_ELEMENTS(action_entries), NULL);
        gtk_action_group_add_toggle_actions(agroup_main, action_tentries,
		G_N_ELEMENTS(action_tentries), NULL);

        ui_manager = gtk_ui_manager_new();
        gtk_ui_manager_insert_action_group(ui_manager, agroup_main, 0);

        if (!gtk_ui_manager_add_ui_from_string(ui_manager, ui_description, -1, &err)) {
                g_message("Building main menu failed : %s", err->message);
                g_error_free(err);
		exit(EXIT_FAILURE);
        }

	/* menu */
        main_menu = gtk_ui_manager_get_widget(ui_manager, "/MainMenu");
	gtk_box_pack_start( GTK_BOX( mainbox ), main_menu, FALSE, FALSE, 0 );
	gtk_widget_show(main_menu);

	if( opt_hmenu ){
		if( NULL != (action = gtk_ui_manager_get_action( ui_manager,
			(gchar*)"/MainMenu/Option/Menu")))

			gtk_toggle_action_set_active(GTK_TOGGLE_ACTION(action), FALSE);
	}

	/* TODO: toolbar */

	/* hotkeys */
	accels = gtk_ui_manager_get_accel_group( ui_manager );
	gtk_window_add_accel_group( GTK_WINDOW(main_win), accels );

	/* player controls */
	playbox = playbox_new();
	gtk_box_pack_start( GTK_BOX( mainbox ), playbox, FALSE, FALSE, 0 );
	gtk_widget_show( playbox );

	if( opt_hbuttons ){
		if( NULL != (action = gtk_ui_manager_get_action( ui_manager,
			"/MainMenu/Option/Buttons" )))

			gtk_toggle_action_set_active(GTK_TOGGLE_ACTION(action), FALSE);
	}


	/* queue window */
	action = gtk_ui_manager_get_action( ui_manager,
			(gchar*)"/MainMenu/Option/Queue");
	queue_win = queuewin_new( action );

	/* track window */
	action = gtk_ui_manager_get_action( ui_manager,
			(gchar*)"/MainMenu/Option/Album");
	tracks_win = curtracks_new( action );

	/* album window */
	action = gtk_ui_manager_get_action( ui_manager,
			(gchar*)"/MainMenu/Option/Artist");
	albums_win = curalbums_new( action );


	/* TODO: statusbar
	 * trackid, servername, username, play_status, sleep, random, gap, filterstat */

	/* dudl */
	memset(&events, 0, sizeof(events));
	events.connect = ev_conn;
	events.disconnect = ev_disc;

	events.nexttrack = ev_nexttrack;
	events.stopped = ev_stopped;
	events.paused = ev_paused;
	events.resumed = ev_resumed;
	events.elapsed = ev_elapsed;

	events.queuefetch = ev_queuefetch;
	events.queueadd = ev_queueadd;
	events.queuedel = ev_queuedel;
	events.queueclear = ev_queueclear;

	duc_setevents( con, &events );
	duc_open( con );

	/* run */
	if( opt_geometry )
		gtk_window_parse_geometry( GTK_WINDOW(main_win), opt_geometry );
	gtk_widget_show( main_win );
	gtk_main();

	return 0;
}
