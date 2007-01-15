#include <string.h>
#include <time.h>
#include <gtk/gtk.h>
#include <dudlc.h>
#include <dudlccmd.h>

dudlc *con = NULL;
char *host = "localhost";
int port = 4445;
char *user = "lirc";
char *pass = "lirc";
int duration;

GtkWidget *ctl_id, *ctl_dur, *ctl_artist, *ctl_album, *ctl_title, *ctl_status;
GtkWidget *ctl_prev, *ctl_stop, *ctl_play, *ctl_pause, *ctl_next;

static void set_status( duc_playstatus stat )
{
	switch(stat){
	  case pl_offline:
		gtk_entry_set_text( GTK_ENTRY(ctl_status), "offline" );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_prev), TRUE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_stop), TRUE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_play), TRUE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_pause), TRUE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_next), TRUE );
		break;
	  case pl_stop:
		gtk_entry_set_text( GTK_ENTRY(ctl_status), "stopped" );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_prev), TRUE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_stop), FALSE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_play), TRUE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_pause), FALSE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_next), TRUE );
		break;
	  case pl_play:
		gtk_entry_set_text( GTK_ENTRY(ctl_status), "playing" );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_prev), TRUE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_stop), TRUE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_play), FALSE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_pause), TRUE );
		gtk_widget_set_sensitive( GTK_WIDGET(ctl_next), TRUE );
		break;
	  case pl_pause:
		gtk_entry_set_text( GTK_ENTRY(ctl_status), "paused" );
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
	duration = 0;
	gtk_entry_set_text( GTK_ENTRY(ctl_id), "" );
	gtk_progress_bar_set_text( GTK_PROGRESS_BAR(ctl_dur), NULL );
	gtk_progress_bar_set_fraction( GTK_PROGRESS_BAR(ctl_dur), 0 );
	gtk_entry_set_text( GTK_ENTRY(ctl_artist), "" );
	gtk_entry_set_text( GTK_ENTRY(ctl_album), "" );
	gtk_entry_set_text( GTK_ENTRY(ctl_title), "" );
}

static void track_set( duc_track *t )
{
	struct tm *tm;
	char buf[32];

	duration = t->duration;
	snprintf( buf, 32, "%d/%d", t->album->id, t->albumnr );
	gtk_entry_set_text( GTK_ENTRY(ctl_id), buf );
	tm = gmtime( (time_t*)&t->duration );
	strftime( buf, 32, "%M:%S", tm );
	gtk_progress_bar_set_text( GTK_PROGRESS_BAR(ctl_dur), buf );
	gtk_entry_set_text( GTK_ENTRY(ctl_artist), t->artist->artist );
	gtk_entry_set_text( GTK_ENTRY(ctl_album), t->album->album );
	gtk_entry_set_text( GTK_ENTRY(ctl_title), t->title );
}

static void cb_disc( dudlc *c )
{
	(void)c;

	set_status( pl_offline );
	track_clear();
}


static void cb_conn( dudlc *c )
{
	duc_playstatus stat;
	duc_track *t;

	stat = duc_cmd_status( c );
	set_status( stat );
	
	if( NULL != (t = duc_cmd_curtrack( c ))){
		track_set( t );
		duc_track_free( t );
	}
}

static void cb_nexttrack( dudlc *c, duc_track *t )
{
	(void)c;
	set_status( pl_play );
	track_set( t );
}

static void cb_stopped( dudlc *c )
{
	(void)c;
	set_status( pl_stop );
	track_clear();
}

static void cb_paused( dudlc *c )
{
	(void)c;
	set_status( pl_pause );
}

static void cb_resumed( dudlc *c )
{
	(void)c;
	set_status( pl_play );
}

static void cb_elapsed( dudlc *c, int r )
{
	(void)c;
	if( duration == 0 )
		gtk_progress_bar_set_fraction( GTK_PROGRESS_BAR(ctl_dur), 0 );
	else if( duration < r )
		gtk_progress_bar_set_fraction( GTK_PROGRESS_BAR(ctl_dur), 1 );
	else 
		gtk_progress_bar_set_fraction( GTK_PROGRESS_BAR(ctl_dur), r / duration );
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
	GtkWidget *ctl_id_l, *ctl_dur_l, *ctl_artist_l, *ctl_album_l, 
		  *ctl_title_l, *ctl_status_l;
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

	ctl_tab = gtk_table_new( 2, 7, FALSE );
	gtk_container_add( GTK_CONTAINER( ctl ), ctl_tab );
	gtk_widget_show( ctl_tab );

	/* status info */
	ctl_id_l = gtk_label_new("ID:");
	gtk_misc_set_alignment( GTK_MISC(ctl_id_l), 1, 0 );
	gtk_table_attach( GTK_TABLE( ctl_tab ), ctl_id_l,
			0, 1, 0, 1, GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
	gtk_widget_show( ctl_id_l );
	ctl_id = gtk_entry_new();
	gtk_editable_set_editable( GTK_EDITABLE(ctl_id), FALSE );
	gtk_entry_set_max_length( GTK_ENTRY(ctl_id), 12 );
	gtk_table_attach( GTK_TABLE( ctl_tab ), ctl_id,
			1, 2, 0, 1, GTK_FILL | GTK_SHRINK, 0, 0, 0);
	gtk_widget_show( ctl_id );

	ctl_dur_l = gtk_label_new("Duration:");
	gtk_misc_set_alignment( GTK_MISC(ctl_dur_l), 1, 0 );
	gtk_table_attach( GTK_TABLE( ctl_tab ), ctl_dur_l,
			0, 1, 1, 2, GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
	gtk_widget_show( ctl_dur_l );
	ctl_dur = gtk_progress_bar_new(); /* TODO: use GtkScale, make it clickable */
	gtk_table_attach( GTK_TABLE( ctl_tab ), ctl_dur,
			1, 2, 1, 2, GTK_EXPAND | GTK_FILL | GTK_SHRINK, 0, 0, 0);
	gtk_widget_show( ctl_dur );

	ctl_album_l = gtk_label_new("Album:");
	gtk_misc_set_alignment( GTK_MISC(ctl_album_l), 1, 0 );
	gtk_table_attach( GTK_TABLE( ctl_tab ), ctl_album_l,
			0, 1, 2, 3, GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
	gtk_widget_show( ctl_album_l );
	ctl_album = gtk_entry_new();
	gtk_editable_set_editable( GTK_EDITABLE(ctl_album), FALSE );
	gtk_table_attach( GTK_TABLE( ctl_tab ), ctl_album,
			1, 2, 2, 3, GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0, 0);
	gtk_widget_show( ctl_album );

	ctl_artist_l = gtk_label_new("Artist:");
	gtk_misc_set_alignment( GTK_MISC(ctl_artist_l), 1, 0 );
	gtk_table_attach( GTK_TABLE( ctl_tab ), ctl_artist_l,
			0, 1, 3, 4, GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
	gtk_widget_show( ctl_artist_l );
	ctl_artist = gtk_entry_new();
	gtk_editable_set_editable( GTK_EDITABLE(ctl_artist), FALSE );
	gtk_table_attach( GTK_TABLE( ctl_tab ), ctl_artist,
			1, 2, 3, 4, GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0, 0);
	gtk_widget_show( ctl_artist );

	ctl_title_l = gtk_label_new("Title:");
	gtk_misc_set_alignment( GTK_MISC(ctl_title_l), 1, 0 );
	gtk_table_attach( GTK_TABLE( ctl_tab ), ctl_title_l,
			0, 1, 4, 5, GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
	gtk_widget_show( ctl_title_l );
	ctl_title = gtk_entry_new();
	gtk_editable_set_editable( GTK_EDITABLE(ctl_title), FALSE );
	gtk_table_attach( GTK_TABLE( ctl_tab ), ctl_title,
			1, 2, 4, 5, GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0, 0);
	gtk_widget_show( ctl_title );

	/* buttons */
	ctl_buttons = gtk_hbox_new( TRUE, 0 );
	gtk_table_attach( GTK_TABLE( ctl_tab ), ctl_buttons,
			0, 2, 5, 6, GTK_EXPAND, GTK_EXPAND, 0, 0);
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

	/* status */
	ctl_status_l = gtk_label_new("Status:");
	gtk_misc_set_alignment( GTK_MISC(ctl_status_l), 1, 0 );
	gtk_table_attach( GTK_TABLE( ctl_tab ), ctl_status_l,
			0, 1, 6, 7, GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
	gtk_widget_show( ctl_status_l );
	ctl_status = gtk_entry_new();
	gtk_editable_set_editable( GTK_EDITABLE(ctl_status), FALSE );
	gtk_table_attach( GTK_TABLE( ctl_tab ), ctl_status,
			1, 2, 6, 7, GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0, 0);
	gtk_widget_show( ctl_status );


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
