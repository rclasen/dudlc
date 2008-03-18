#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <dudlc.h>
#include <dudlccmd.h>

int progress_seekable = 1;
int duration = 0;

GtkWidget *ctl_title, *ctl_album, *ctl_dur, *ctl_prog;
GtkWidget *ctl, *ctl_menu, *ctl_rowb, *ctl_status;
GtkWidget *ctl_prev, *ctl_stop, *ctl_play, *ctl_pause, *ctl_next;

static void shrink_y( GtkWindow *win )
{
	gint x;
	gtk_window_get_size( win, &x, NULL );
	gtk_window_resize( win, x, 1 );
}

static gchar *time_fmt( int value )
{
	return g_strdup_printf( "%d:%02d", (int)value/60, (int)value % 60 );
}

static void menu_set( gboolean stat )
{
	if( stat )
		gtk_widget_show(GTK_WIDGET(ctl_menu));
	else {
		gtk_widget_hide(GTK_WIDGET(ctl_menu));
		shrink_y(GTK_WINDOW(ctl));
	}
}

static void menu_toggle( GtkAction *action, gpointer data ) 
{
	(void)data;
	menu_set( gtk_toggle_action_get_active( GTK_TOGGLE_ACTION(action)));
}

static void buttons_toggle( GtkAction *action, gpointer data ) 
{
	(void)data;
	if(gtk_toggle_action_get_active(GTK_TOGGLE_ACTION(action)))
		gtk_widget_show(ctl_rowb);
	else {
		gtk_widget_hide(ctl_rowb);
		shrink_y(GTK_WINDOW(ctl));
	}
}


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


static void status_set( duc_playstatus stat )
{
	switch(stat){
	  case pl_offline:
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_prog), FALSE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_prev), TRUE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_stop), TRUE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_play), TRUE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_pause), TRUE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_next), TRUE );
		break;
	  case pl_stop:
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_prog), FALSE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_prev), TRUE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_stop), FALSE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_play), TRUE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_pause), FALSE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_next), TRUE );
		break;
	  case pl_play:
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_prog), TRUE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_prev), TRUE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_stop), TRUE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_play), FALSE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_pause), TRUE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_next), TRUE );
		break;
	  case pl_pause:
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_prog), TRUE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_prev), TRUE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_stop), TRUE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_play), TRUE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_pause), FALSE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_next), TRUE );
		break;
	}
}

static void track_clear( void )
{
	gtk_entry_set_text( GTK_ENTRY(ctl_title), "" );
	gtk_entry_set_text( GTK_ENTRY(ctl_album), "" );
	gtk_entry_set_text( GTK_ENTRY(ctl_dur), "" );
	gtk_range_set_value( GTK_RANGE(ctl_prog), 0 );
}

static void track_set( duc_track *t )
{
	GtkObject *adj;
	gchar *buf, *ubuf;


	/* track: 0/2684 */
	buf = g_strdup_printf( "%s: %s", t->artist->artist, t->title );
	ubuf = g_locale_to_utf8( buf, -1, NULL, NULL, NULL);
	gtk_entry_set_text( GTK_ENTRY(ctl_title), ubuf );
	g_free(ubuf);
	g_free(buf);

	ubuf = g_locale_to_utf8( t->album->album, -1, NULL, NULL, NULL);
	gtk_entry_set_text( GTK_ENTRY(ctl_album), ubuf );
	g_free(ubuf);

	duration = t->duration;
	adj = gtk_adjustment_new( 0, 0, duration, 1, 10, 0 );
	gtk_range_set_adjustment( GTK_RANGE(ctl_prog), GTK_ADJUSTMENT(adj) );

	buf = time_fmt( duration );
	gtk_entry_set_text( GTK_ENTRY(ctl_dur), buf );
	g_free(buf);
}

static void progress_set( int r )
{
	progress_seekable = 0;
	if( duration == 0 )
		gtk_range_set_value( GTK_RANGE(ctl_prog), 0 );
	else if( duration < r )
		gtk_range_set_value( GTK_RANGE(ctl_prog), duration );
	else 
		gtk_range_set_value( GTK_RANGE(ctl_prog), r );
	progress_seekable = 1;
}


static gboolean cb_read( GIOChannel *source, GIOCondition cond, gpointer data)
{
	(void)source;
	(void)cond;
	duc_poll( (dudlc*)data );
	return TRUE;
}

static void cb_disc( dudlc *c )
{
	while( g_source_remove_by_user_data( c ) );
	status_set( pl_offline );
	track_clear();
}


static void cb_conn( dudlc *c )
{
	GIOChannel *cchan;
	duc_playstatus stat;
	duc_track *t;

	cchan = g_io_channel_unix_new(duc_fd(c));
	g_io_add_watch(cchan, G_IO_IN | G_IO_HUP | G_IO_ERR, cb_read, c );

	stat = duc_cmd_status( c );
	status_set( stat );
	
	if( NULL != (t = duc_cmd_curtrack( c ))){
		track_set( t );
		duc_track_free( t );
	}
}

static void cb_nexttrack( dudlc *c, duc_track *t )
{
	(void)c;
	status_set( pl_play );
	track_set( t );
}

static void cb_stopped( dudlc *c )
{
	(void)c;
	status_set( pl_stop );
	track_clear();
}

static void cb_paused( dudlc *c )
{
	(void)c;
	status_set( pl_pause );
}

static void cb_resumed( dudlc *c )
{
	(void)c;
	status_set( pl_play );
}

static void cb_elapsed( dudlc *c, int r )
{
	(void)c;
	progress_set( r );
}

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

static gchar *ctl_prog__format( GtkWidget *widget, gdouble value )
{
	(void)widget;
	return time_fmt( value );
}

static void ctl_prog__changed( GtkButton *widget, gpointer data )
{
	GtkAdjustment *adj;
	int val;

	if( progress_seekable ){
		adj = gtk_range_get_adjustment( GTK_RANGE(widget) );
		val = gtk_adjustment_get_value( GTK_ADJUSTMENT(adj) );
		duc_cmd_jump( (dudlc*)data, val );
	}
}

static void ctl_prev__click( GtkButton *widget, gpointer data )
{
	(void)widget;
	duc_cmd_prev( (dudlc*)data );
}

static void ctl_stop__click( GtkButton *widget, gpointer data )
{
	(void)widget;
	duc_cmd_stop( (dudlc*)data );
}

static void ctl_play__click( GtkButton *widget, gpointer data )
{
	(void)widget;
	duc_cmd_play( (dudlc*)data );
}

static void ctl_pause__click( GtkButton *widget, gpointer data )
{
	(void)widget;
	duc_cmd_pause( (dudlc*)data );
}

static void ctl_next__click( GtkButton *widget, gpointer data )
{
	(void)widget;
	duc_cmd_next( (dudlc*)data );
}

static void def_string( char **dst, GKeyFile *kf, char *key, char *def )
{
	GError *err = NULL;

	if( *dst )
		return;

	if( kf )
		*dst = g_key_file_get_string( kf, "dudlc", key, &err );

	if( ! *dst )
		*dst = def;
}

static void def_integer( int *dst, GKeyFile *kf, char *key, int def )
{
	GError *err = NULL;

	if( *dst >= 0 )
		return;

	if( kf )
		*dst = g_key_file_get_integer( kf, "dudlc", key, &err );
	if( err && err->code == G_KEY_FILE_ERROR_INVALID_VALUE )
		g_printerr( "invalid data for %s: %s", key, err->message );

	if( err || *dst < 0 )
		*dst = def;
}



int main( int argc, char **argv )
{
	char *opt_host = NULL;
	int opt_port = -1;
	char *opt_user = NULL;
	char *opt_pass = NULL;
	char *opt_cfg = NULL;
	duc_events events;
	dudlc *con = NULL;
	GtkWidget *ctl_mainbox, *ctl_rowa;
	GtkWidget *ctl_buttons;
	GError *err = NULL;
	GKeyFile *keyfile = NULL;
	GOptionEntry option_entries[] = {
		{ "host",	'H', 0,
			G_OPTION_ARG_STRING,	&opt_host,
			"dudl server name to connect to",
			"hostname" },
		{ "port",	'p', 0,
			G_OPTION_ARG_INT,	&opt_port,
			"TCP port on dudl server to connect to",
			"port" },
		{ "user",	'u', 0,
			G_OPTION_ARG_STRING,	&opt_user,
			"dudl user name",
			"username" },
		{ "pass",	'P', 0,
			G_OPTION_ARG_STRING,	&opt_pass,
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
	};
	GtkToggleActionEntry action_tentries[] = {
		{ "OptButtons", NULL, "show _Buttons", "<control>B", 
			"show/hide player buttons", G_CALLBACK(buttons_toggle), 1 },
		{ "OptStatusbar", NULL, "show _Statusbar", NULL, 
			"show/hide status bar", G_CALLBACK(status_toggle), 0 },
		{ "OptMenu", NULL, "show _Menu", "<control>M", 
			"show/hide menu", G_CALLBACK(menu_toggle), 0 },
	};
	const char * ui_description =
		"<ui>"
		"  <menubar name='MainMenu'>"
		"    <menu action='FileMenu'>"
		"      <menuitem action='Quit'/>"
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
	/* TODO: split into several files */
	/* TODO: use fill-level instead of value for progress - req. gtk 2.12*/
	/* TODO: keyboard shortcuts */
	/* TODO: icon */
	/* TODO: check window/icon names */
	/* TODO: view/edit frontend for user, clients, tags, filtertop,
	 * history, queue, track_search, album_search, artist_search,
	 */
	/* TODO: command window: classic/using gtk for results */
	/* TODO: context menu */
	/* TODO: retry connect every x seconds */

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

	keyfile = g_key_file_new();
	if( ! g_key_file_load_from_file( keyfile, opt_cfg, G_KEY_FILE_NONE, &err )){
		if( err->domain == G_KEY_FILE_ERROR && 
				err->code == G_KEY_FILE_ERROR_NOT_FOUND )
			g_printerr( "error reading config %s: %s", 
					opt_cfg, err->message );
		g_clear_error( &err );
		g_key_file_free( keyfile );
		keyfile = NULL;
	}

	def_string( &opt_host, keyfile, "host", "localhost" );
	def_integer( &opt_port, keyfile, "port", 4445 );
	def_string( &opt_user, keyfile, "user", "guest" );
	def_string( &opt_pass, keyfile, "pass", "guest" );

	if( keyfile ){
		g_key_file_free( keyfile );
		keyfile = NULL;
	}

	/* dudl part one */
	con = duc_new( opt_host, opt_port );
	duc_setauth( con, opt_user, opt_pass );


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

	/* menu */
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

        ctl_menu = gtk_ui_manager_get_widget(ui_manager, "/MainMenu");
	gtk_box_pack_start( GTK_BOX( ctl_mainbox ), ctl_menu, FALSE, FALSE, 0 );
	accels = gtk_ui_manager_get_accel_group( ui_manager );
	gtk_window_add_accel_group( GTK_WINDOW(ctl), accels );

	gtk_widget_hide( ctl_menu );

	/* status info */
	/* TODO: maybe use GtkLabel instead of GtkEntry
	 * _set_selctable( TRUE ); 
	 * update of text-fields may cause window enlargment
	 */
	ctl_title = gtk_entry_new();
	gtk_entry_set_alignment( GTK_ENTRY(ctl_title), 0 );
	gtk_editable_set_editable( GTK_EDITABLE(ctl_title), FALSE );
	gtk_box_pack_start_defaults( GTK_BOX( ctl_mainbox ), ctl_title );
	gtk_widget_show( ctl_title );

	ctl_rowa = gtk_hbox_new( FALSE, 0 );
	gtk_container_add( GTK_CONTAINER( ctl_mainbox ), ctl_rowa );
	gtk_widget_show( ctl_rowa );

	ctl_album = gtk_entry_new();
	gtk_entry_set_alignment( GTK_ENTRY(ctl_album), 0 );
	gtk_editable_set_editable( GTK_EDITABLE(ctl_album), FALSE );
	gtk_box_pack_start_defaults( GTK_BOX( ctl_rowa ), ctl_album );
	gtk_widget_show( ctl_album );

	/* TODO: ?display duration as label right of progress */
	ctl_dur = gtk_entry_new();
	gtk_entry_set_alignment( GTK_ENTRY(ctl_dur), 1 );
	gtk_entry_set_width_chars( GTK_ENTRY(ctl_dur), 5 );
	gtk_editable_set_editable( GTK_EDITABLE(ctl_dur), FALSE );
	gtk_box_pack_end( GTK_BOX( ctl_rowa ), ctl_dur,
			FALSE, TRUE, 0);
	gtk_widget_show( ctl_dur );

	/* TODO: more details 
	 * IDs, Tags, filter, album_artist */

	ctl_prog = gtk_hscale_new_with_range(0, 1, 1);
	gtk_scale_set_value_pos( GTK_SCALE(ctl_prog), GTK_POS_LEFT );
	gtk_range_set_update_policy( GTK_RANGE(ctl_prog), GTK_UPDATE_DISCONTINUOUS );
	gtk_signal_connect( GTK_OBJECT( ctl_prog ), "format-value",
			G_CALLBACK( ctl_prog__format ), NULL);
	gtk_signal_connect( GTK_OBJECT( ctl_prog ), "value-changed",
			G_CALLBACK( ctl_prog__changed ), con);
	gtk_box_pack_start_defaults( GTK_BOX( ctl_mainbox ), ctl_prog );
	gtk_widget_show( ctl_prog );

	/* buttons */
	/* TODO: add button images */
	ctl_rowb = gtk_hbox_new( FALSE, 0 );
	gtk_box_pack_start( GTK_BOX( ctl_mainbox ), ctl_rowb,
			TRUE, FALSE, 0 );
	gtk_widget_show( ctl_rowb );
	
	ctl_buttons = gtk_hbox_new( TRUE, 0 );
	gtk_box_pack_start( GTK_BOX( ctl_rowb ), ctl_buttons,
			FALSE, FALSE, 0 );
	gtk_widget_show( ctl_buttons );

	ctl_prev = gtk_button_new_with_label( "<<" );
	gtk_signal_connect( GTK_OBJECT( ctl_prev ), "clicked",
			G_CALLBACK( ctl_prev__click ), con);
	gtk_box_pack_start( GTK_BOX( ctl_buttons ), ctl_prev,
			FALSE, TRUE, 0);
	gtk_widget_show( ctl_prev );

	ctl_stop = gtk_button_new_with_label( "[]" );
	gtk_signal_connect( GTK_OBJECT( ctl_stop ), "clicked",
			G_CALLBACK( ctl_stop__click ), con);
	gtk_box_pack_start( GTK_BOX( ctl_buttons ), ctl_stop,
			FALSE, TRUE, 0);
	gtk_widget_show( ctl_stop );

	ctl_pause = gtk_button_new_with_label( "\"" );
	gtk_signal_connect( GTK_OBJECT( ctl_pause ), "clicked",
			G_CALLBACK( ctl_pause__click ), con);
	gtk_box_pack_start( GTK_BOX( ctl_buttons ), ctl_pause,
			FALSE, TRUE, 0);
	gtk_widget_show( ctl_pause );

	ctl_play = gtk_button_new_with_label( ">" );
	gtk_signal_connect( GTK_OBJECT( ctl_play ), "clicked",
			G_CALLBACK( ctl_play__click ), con);
	gtk_box_pack_start( GTK_BOX( ctl_buttons ), ctl_play,
			FALSE, TRUE, 0);
	gtk_widget_show( ctl_play );

	ctl_next = gtk_button_new_with_label( ">>" );
	gtk_signal_connect( GTK_OBJECT( ctl_next ), "clicked",
			G_CALLBACK( ctl_next__click ), con);
	gtk_box_pack_start( GTK_BOX( ctl_buttons ), ctl_next,
			FALSE, TRUE, 0);
	gtk_widget_show( ctl_next );

	/* TODO: statusbar
	 * servername, username, play_status, sleep, random, gap, filterstat */

	/* dudl */
	memset(&events, 0, sizeof(events));
	duc_setevents( con, &events );
	events.connect = cb_conn;
	events.disconnect = cb_disc;
	events.nexttrack = cb_nexttrack;
	events.stopped = cb_stopped;
	events.paused = cb_paused;
	events.resumed = cb_resumed;
	events.elapsed = cb_elapsed;

	duc_open( con );

	/* run */
	gtk_widget_show( ctl );
	gtk_main();

	return 0;
}
