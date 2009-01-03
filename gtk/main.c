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
static GtkWidget *ctl, *ctl_menu /*, *ctl_status */;

/************************************************************
 * dudl interaction
 */

static gboolean cb_dudl_poll( GIOChannel *source, GIOCondition cond, gpointer data)
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
	playbox_detail_clear();
}

static void ev_conn( dudlc *c )
{
	GIOChannel *cchan;
	duc_playstatus stat;
	duc_track *t;

	cchan = g_io_channel_unix_new(duc_fd(c));
	g_io_add_watch(cchan, G_IO_IN | G_IO_HUP | G_IO_ERR, cb_dudl_poll, c );

	stat = duc_cmd_status( c );
	playbox_buttons_enable( stat );
	
	if( NULL != (t = duc_cmd_curtrack( c ))){
		playbox_detail_update( t );
		duc_track_free( t );
	}
}

static void ev_nexttrack( dudlc *c, duc_track *t )
{
	(void)c;
	playbox_buttons_enable( pl_play );
	playbox_detail_update( t );
}

static void ev_stopped( dudlc *c )
{
	(void)c;
	playbox_buttons_enable( pl_stop );
	playbox_detail_clear();
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
		gtk_widget_show(GTK_WIDGET(ctl_menu));
	else {
		gtk_widget_hide(GTK_WIDGET(ctl_menu));
		shrink_y(GTK_WINDOW(ctl));
	}
}

static void buttons_toggle( GtkAction *action, gpointer data ) 
{
	gboolean show;
	(void)data;
	
	show = gtk_toggle_action_get_active(GTK_TOGGLE_ACTION(action));

	playbox_buttons_show( show );
	if(! show )
		shrink_y(GTK_WINDOW(ctl));
}

static void show_album( GtkAction *action, gpointer data ) 
{
	duc_track *track;
	duc_it_track *it;
	GtkWidget *win;

	(void)action;
	(void)data;

	if( NULL == (track = duc_cmd_curtrack( con )))
		return;

	if( NULL == (it = duc_cmd_tracksalbum( con, track->album->id ))){
		duc_track_free(track);
		return;
	}

	win = childwindow_new( "track list", track_list_new_with_list(it) );
	gtk_window_set_default_size(GTK_WINDOW(win), 500, 300);
	gtk_widget_show( win );

	duc_track_free(track);
	duc_it_track_done(it);
}

/*
static void status_toggle( GtkAction *action, gpointer data ) 
{
	(void)data;
	if(gtk_toggle_action_get_active(GTK_TOGGLE_ACTION(action))) 
		gtk_widget_show(ctl_status);
	else {
		gtk_widget_hide(ctl_status);
		shrink_y(GTK_WINDOW(ctl));
	}
}
*/

static int ctl__delete( GtkWidget *widget, GdkEvent  *event, gpointer   data )
{
	(void)widget;
	(void)event;
	(void)data;
	return FALSE;
}

static void ctl__destroy( GtkWidget *widget, gpointer data )
{
	(void)widget;
	(void)data;
	gtk_main_quit();
}

int main( int argc, char **argv )
{
	options opt;
	char *opt_cfg = NULL;
	duc_events events;
	GtkWidget *ctl_mainbox;
	GtkWidget *ctl_playbox;
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
		{ NULL, 0, 0, 0, NULL, NULL, NULL } 
	};
	GtkActionEntry action_entries[] = {
		{ "FileMenu", NULL, "_File", NULL, NULL, NULL },
		{ "Quit", GTK_STOCK_QUIT, "_Quit", "<control>Q", 
			"Quit the program", G_CALLBACK(gtk_main_quit) },

		{ "OptionMenu", NULL, "_Options", NULL, NULL, NULL },

		{ "ShowMenu", NULL, "_Show", NULL, NULL, NULL },
		{ "ShowAlbum", NULL, "show _Album", "<control>L",
			"show tracks of current album", G_CALLBACK(show_album) },
	};
	GtkToggleActionEntry action_tentries[] = {
		{ "OptButtons", NULL, "show _Buttons", "<control>B", 
			"show/hide player buttons", G_CALLBACK(buttons_toggle), 1 },
/*
		{ "OptStatusbar", NULL, "show _Statusbar", NULL, 
			"show/hide status bar", G_CALLBACK(status_toggle), 0 },
*/
		{ "OptMenu", NULL, "show _Menu", "<control>M", 
			"show/hide menu", G_CALLBACK(menu_toggle), 0 },
	};
	const char * ui_description =
		"<ui>"
		"  <menubar name='MainMenu'>"
		"    <menu action='FileMenu'>"
		"      <menuitem action='Quit'/>"
		"    </menu>"
		"    <menu action='ShowMenu'>"
		"      <menuitem action='ShowAlbum'/>"
		"    </menu>"
		"    <menu action='OptionMenu'>"
		"      <menuitem action='OptMenu'/>"
		"      <menuitem action='OptButtons'/>"
		/* TODO: "      <menuitem action='OptionStatusbar'/>" */
		"    </menu>"
		"  </menubar>"	
		"</ui>";
        GtkActionGroup *agroup_main;
	GtkAccelGroup *accels;
	GtkUIManager *ui_manager;


	/* *TODO: error handling */
	/* TODO: keyboard shortcuts */
	/* TODO: icon */
	/* TODO: check window/icon names */
	/* TODO: view/edit frontend for user, clients, tags, filtertop,
	 * history, queue, track_search, album_search, artist_search,
	 */
	/* TODO: command window: classic/using gtk for results */
	/* TODO: context menu */
	/* TODO: retry connect every x seconds */

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
	ctl = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	gtk_window_set_title( GTK_WINDOW(ctl), "dudlc" );
	gtk_signal_connect( GTK_OBJECT( ctl ), "delete_event", 
			G_CALLBACK( ctl__delete ), NULL);
	gtk_signal_connect( GTK_OBJECT( ctl ), "destroy", 
			G_CALLBACK( ctl__destroy ), NULL);

	ctl_mainbox = gtk_vbox_new( FALSE, 0 );
	gtk_container_add( GTK_CONTAINER( ctl ), ctl_mainbox );
	gtk_widget_show( ctl_mainbox );

	/* hotkeys */
	agroup_main = gtk_action_group_new("MainWindowActions");
        gtk_action_group_add_actions(agroup_main, action_entries, 
		G_N_ELEMENTS(action_entries), NULL);
        gtk_action_group_add_toggle_actions(agroup_main, action_tentries, 
		G_N_ELEMENTS(action_tentries), NULL);
        
        ui_manager = gtk_ui_manager_new();
        gtk_ui_manager_insert_action_group(ui_manager, agroup_main, 0);
        
	/* menu */
        if (!gtk_ui_manager_add_ui_from_string(ui_manager, ui_description, -1, &err)) {
                g_message("Building main menu failed : %s", err->message);
                g_error_free(err); 
		exit(EXIT_FAILURE);
        }

        ctl_menu = gtk_ui_manager_get_widget(ui_manager, "/MainMenu");
	gtk_box_pack_start( GTK_BOX( ctl_mainbox ), ctl_menu, FALSE, FALSE, 0 );
	accels = gtk_ui_manager_get_accel_group( ui_manager );
	gtk_window_add_accel_group( GTK_WINDOW(ctl), accels );

	gtk_widget_hide( ctl_menu );

	/* player controls */
	ctl_playbox = playbox_new();
	gtk_box_pack_start( GTK_BOX( ctl_mainbox ), ctl_playbox, FALSE, FALSE, 0 );
	gtk_widget_show( ctl_playbox );

	/* TODO: statusbar
	 * servername, username, play_status, sleep, random, gap, filterstat */

	/* dudl */
	memset(&events, 0, sizeof(events));
	events.connect = ev_conn;
	events.disconnect = ev_disc;
	events.nexttrack = ev_nexttrack;
	events.stopped = ev_stopped;
	events.paused = ev_paused;
	events.resumed = ev_resumed;
	events.elapsed = ev_elapsed;

	duc_setevents( con, &events );
	duc_open( con );

	/* run */
	gtk_widget_show( ctl );
	gtk_main();

	return 0;
}
