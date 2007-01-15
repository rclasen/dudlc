#include <string.h>
#include <gtk/gtk.h>
#include <dudlc.h>
#include <dudlccmd.h>

#define PROG_MAX 10000

dudlc *con = NULL;
char *host = "localhost";
int port = 4445;
char *user = "lirc";
char *pass = "lirc";

int duration;

GtkWidget *ctl_title, *ctl_album, *ctl_dur, *ctl_prog;
GtkWidget *ctl_prev, *ctl_stop, *ctl_play, *ctl_pause, *ctl_next;

static void status_set( duc_playstatus stat )
{
	switch(stat){
	  case pl_offline:
		// TODO: gtk_entry_set_text( GTK_ENTRY(ctl_status), "offline" );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_prev), TRUE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_stop), TRUE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_play), TRUE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_pause), TRUE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_next), TRUE );
		break;
	  case pl_stop:
		// TODO: gtk_entry_set_text( GTK_ENTRY(ctl_status), "stopped" );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_prev), TRUE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_stop), FALSE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_play), TRUE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_pause), FALSE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_next), TRUE );
		break;
	  case pl_play:
		// TODO: gtk_entry_set_text( GTK_ENTRY(ctl_status), "playing" );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_prev), TRUE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_stop), TRUE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_play), FALSE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_pause), TRUE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_next), TRUE );
		break;
	  case pl_pause:
		// TODO: gtk_entry_set_text( GTK_ENTRY(ctl_status), "paused" );
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
	gtk_label_set_text( GTK_LABEL(ctl_title), "" );
	gtk_label_set_text( GTK_LABEL(ctl_album), "" );
	gtk_label_set_text( GTK_LABEL(ctl_dur), NULL );
	gtk_range_set_value( GTK_RANGE(ctl_prog), 0 );
}

static void track_set( duc_track *t )
{
	gchar *buf;

	buf = g_strdup_printf( "%s: %s", t->artist->artist, t->title );
	gtk_label_set_text( GTK_LABEL(ctl_title), buf );
	g_free(buf);

	gtk_label_set_text( GTK_LABEL(ctl_album), t->album->album );

	duration = t->duration;
	buf = g_strdup_printf( "%d:%02d", (int)duration/60, duration % 60 );
	gtk_label_set_text( GTK_LABEL(ctl_dur), buf );
	g_free(buf);
}

static void cb_disc( dudlc *c )
{
	(void)c;

	status_set( pl_offline );
	track_clear();
}


static void cb_conn( dudlc *c )
{
	duc_playstatus stat;
	duc_track *t;

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
	if( duration == 0 )
		gtk_range_set_value( GTK_RANGE(ctl_prog), 0 );
	else if( duration < r )
		gtk_range_set_value( GTK_RANGE(ctl_prog), PROG_MAX );
	else 
		gtk_range_set_value( GTK_RANGE(ctl_prog), 
				PROG_MAX * r / duration );
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
	GtkAdjustment *adj;
	int val;

	(void)widget;
	(void)value;
	adj = gtk_range_get_adjustment( GTK_RANGE(widget) );
	val = gtk_adjustment_get_value( GTK_ADJUSTMENT(adj) ) / PROG_MAX * duration;
	g_print( "dur/jump: %d/%d\n", duration, val );
	return g_strdup_printf( "%d:%02d", (int)val/60, (int)val % 60 );
}

static void ctl_prog__changed( GtkButton *widget, gpointer data )
{
	GtkAdjustment *adj;
	int val;

	(void)data;
	adj = gtk_range_get_adjustment( GTK_RANGE(widget) );
	val = gtk_adjustment_get_value( GTK_ADJUSTMENT(adj) ) / PROG_MAX * duration;
	g_print( "dur/jump: %d/%d\n", duration, val );
	duc_cmd_jump( con, val );
}

static void ctl_prev__click( GtkButton *widget, gpointer data )
{
	(void)widget;
	(void)data;
	duc_cmd_prev( con );
}

static void ctl_stop__click( GtkButton *widget, gpointer data )
{
	(void)widget;
	(void)data;
	duc_cmd_stop( con );
}

static void ctl_play__click( GtkButton *widget, gpointer data )
{
	(void)widget;
	(void)data;
	duc_cmd_play( con );
}

static void ctl_pause__click( GtkButton *widget, gpointer data )
{
	(void)widget;
	(void)data;
	duc_cmd_pause( con );
}

static void ctl_next__click( GtkButton *widget, gpointer data )
{
	(void)widget;
	(void)data;
	duc_cmd_next( con );
}

int main( int argc, char **argv )
{
	GtkWidget *ctl;
	GtkWidget *ctl_tab;
	GtkWidget *ctl_buttons;
	duc_events events;

	/* TODO: parse command line options */
	/* TODO: config file */
	/* TODO: keyboard shortcuts */
	gtk_init( &argc, &argv );

	/* main window */
	ctl = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	gtk_window_set_title( GTK_WINDOW(ctl), "dudlc" );
	/* gtk_widget_set_size_request( GTK_WIDGET( ctl), x, y ) */
	gtk_signal_connect( GTK_OBJECT( ctl ), "delete_event", 
			GTK_SIGNAL_FUNC( ctl__delete ), NULL);
	gtk_signal_connect( GTK_OBJECT( ctl ), "destroy", 
			GTK_SIGNAL_FUNC( ctl__destroy ), NULL);

	ctl_tab = gtk_table_new( 4, 7, FALSE );
	gtk_container_add( GTK_CONTAINER( ctl ), ctl_tab );
	gtk_widget_show( ctl_tab );

	/* status info */
	ctl_title = gtk_label_new("");
	gtk_misc_set_alignment( GTK_MISC(ctl_title), 0, 0 );
	gtk_table_attach( GTK_TABLE( ctl_tab ), ctl_title,
			0, 4, 0, 1, GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0, 0);
	gtk_widget_show( ctl_title );

	ctl_album = gtk_label_new("");
	gtk_misc_set_alignment( GTK_MISC(ctl_album), 0, 0 );
	gtk_table_attach( GTK_TABLE( ctl_tab ), ctl_album,
			0, 2, 1, 2, GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0, 0);
	gtk_widget_show( ctl_album );

	ctl_dur = gtk_label_new("");
	gtk_misc_set_alignment( GTK_MISC(ctl_dur), 1, 0 );
	gtk_table_attach( GTK_TABLE( ctl_tab ), ctl_dur,
			2, 3, 1, 2, GTK_FILL, 0, 0, 0);
	gtk_widget_show( ctl_dur );

	// TODO: more details

	ctl_prog = gtk_hscale_new_with_range(0, PROG_MAX, 1);
	gtk_scale_set_value_pos( GTK_SCALE(ctl_prog), GTK_POS_RIGHT );
	//gtk_scale_set_draw_value( GTK_SCALE(ctl_prog), FALSE );
	gtk_range_set_update_policy( GTK_RANGE(ctl_prog), GTK_UPDATE_DISCONTINUOUS );
	gtk_signal_connect( GTK_OBJECT( ctl_prog ), "format-value",
			GTK_SIGNAL_FUNC( ctl_prog__format ), NULL);
	gtk_signal_connect( GTK_OBJECT( ctl_prog ), "value-changed",
			GTK_SIGNAL_FUNC( ctl_prog__changed ), NULL);
	gtk_table_attach( GTK_TABLE( ctl_tab ), ctl_prog,
			0, 4, 2, 3, GTK_EXPAND | GTK_FILL | GTK_SHRINK, 0, 0, 0);
	gtk_widget_show( ctl_prog );

	/* buttons */
	ctl_buttons = gtk_hbox_new( TRUE, 0 );
	gtk_table_attach( GTK_TABLE( ctl_tab ), ctl_buttons,
			0, 4, 3, 4, GTK_EXPAND, GTK_EXPAND, 0, 0);
	gtk_widget_show( ctl_buttons );

	ctl_prev = gtk_button_new_with_label( "<<" );
	gtk_signal_connect( GTK_OBJECT( ctl_prev ), "clicked",
			GTK_SIGNAL_FUNC( ctl_prev__click ), NULL);
	gtk_box_pack_start( GTK_BOX( ctl_buttons ), ctl_prev,
			FALSE, TRUE, 0);
	gtk_widget_show( ctl_prev );

	ctl_stop = gtk_button_new_with_label( "[]" );
	gtk_signal_connect( GTK_OBJECT( ctl_stop ), "clicked",
			GTK_SIGNAL_FUNC( ctl_stop__click ), NULL);
	gtk_box_pack_start( GTK_BOX( ctl_buttons ), ctl_stop,
			FALSE, TRUE, 0);
	gtk_widget_show( ctl_stop );

	ctl_play = gtk_button_new_with_label( ">" );
	gtk_signal_connect( GTK_OBJECT( ctl_play ), "clicked",
			GTK_SIGNAL_FUNC( ctl_play__click ), NULL);
	gtk_box_pack_start( GTK_BOX( ctl_buttons ), ctl_play,
			FALSE, TRUE, 0);
	gtk_widget_show( ctl_play );

	ctl_pause = gtk_button_new_with_label( "\"" );
	gtk_signal_connect( GTK_OBJECT( ctl_pause ), "clicked",
			GTK_SIGNAL_FUNC( ctl_pause__click ), NULL);
	gtk_box_pack_start( GTK_BOX( ctl_buttons ), ctl_pause,
			FALSE, TRUE, 0);
	gtk_widget_show( ctl_pause );

	ctl_next = gtk_button_new_with_label( ">>" );
	gtk_signal_connect( GTK_OBJECT( ctl_next ), "clicked",
			GTK_SIGNAL_FUNC( ctl_next__click ), NULL);
	gtk_box_pack_start( GTK_BOX( ctl_buttons ), ctl_next,
			FALSE, TRUE, 0);
	gtk_widget_show( ctl_next );

	// TODO: statusbar

	/* dudl */
	con = duc_new( host, port );

	memset(&events, 0, sizeof(events));
	duc_setevents( con, &events );
	events.connect = cb_conn;
	events.disconnect = cb_disc;
	events.nexttrack = cb_nexttrack;
	events.stopped = cb_stopped;
	events.paused = cb_paused;
	events.resumed = cb_resumed;
	events.elapsed = cb_elapsed;

	duc_setauth( con, user, pass );
	duc_open( con );

	/* run */
	gtk_widget_show( ctl );
	gtk_main();

	return 0;
}
