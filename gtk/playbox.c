/*
 * Copyright (c) 2008 Rainer Clasen
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

#include <gtk/gtk.h>
#include <dudlc.h>

#include "common.h"

/* TODO: make this a widget */

/* track detail widgets */
static GtkWidget *ctl_title, *ctl_album, *ctl_dur, *ctl_prog;

/* buttons */
static GtkWidget *ctl_buttons;
static GtkWidget *ctl_prev, *ctl_stop, *ctl_play, *ctl_pause, *ctl_next;

static int progress_seekable = 1;
static int duration = 0;

void playbox_buttons_show( gboolean show )
{
	if(show)
		gtk_widget_show(ctl_buttons);
	else
		gtk_widget_hide(ctl_buttons);
}

void playbox_buttons_enable( duc_playstatus stat )
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

static gchar *time_fmt( int value )
{
	return g_strdup_printf( "%d:%02d", (int)value/60, (int)value % 60 );
}

void playbox_detail_update( duc_track *t )
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

void playbox_detail_clear( void )
{
	gtk_entry_set_text( GTK_ENTRY(ctl_title), "" );
	gtk_entry_set_text( GTK_ENTRY(ctl_album), "" );
	gtk_entry_set_text( GTK_ENTRY(ctl_dur), "" );
	gtk_range_set_value( GTK_RANGE(ctl_prog), 0 );
}

void playbox_progress_update( int r )
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

/************************************************************
 * widget signal handler
 */

static gchar *ctl_prog__format( GtkWidget *widget, gdouble value )
{
	(void)widget;
	return time_fmt( value );
}

static void ctl_prog__changed( GtkButton *widget, gpointer data )
{
	GtkAdjustment *adj;
	int val;

	(void)data;
	if( progress_seekable ){
		adj = gtk_range_get_adjustment( GTK_RANGE(widget) );
		val = gtk_adjustment_get_value( GTK_ADJUSTMENT(adj) );
		duc_cmd_jump( con, val );
	}
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


/************************************************************
 * widget creation
 */

static GtkWidget *detail2_new( void )
{
	GtkWidget *box;

	box = gtk_hbox_new( FALSE, 0 );

	ctl_album = gtk_entry_new();
	gtk_entry_set_alignment( GTK_ENTRY(ctl_album), 0 );
	gtk_editable_set_editable( GTK_EDITABLE(ctl_album), FALSE );
	gtk_box_pack_start_defaults( GTK_BOX( box ), ctl_album );
	gtk_widget_show( ctl_album );

	/* TODO: ?display duration as label right of progress */
	ctl_dur = gtk_entry_new();
	gtk_entry_set_alignment( GTK_ENTRY(ctl_dur), 1 );
	gtk_entry_set_width_chars( GTK_ENTRY(ctl_dur), 5 );
	gtk_editable_set_editable( GTK_EDITABLE(ctl_dur), FALSE );
	gtk_box_pack_end( GTK_BOX( box ), ctl_dur,
			FALSE, TRUE, 0);
	gtk_widget_show( ctl_dur );

	return box;
}

static GtkWidget *playbox_buttons_new( void )
{
	GtkWidget *box;

	/*
	 * TODO: add button images:
	 * gtk_button_new_from_stock ()
	 * GTK_STOCK_MEDIA_PREVIOUS
	 * GTK_STOCK_MEDIA_STOP
	 * GTK_STOCK_MEDIA_PAUSE
	 * GTK_STOCK_MEDIA_PLAY
	 * GTK_STOCK_MEDIA_NEXT
	 * http://www.harshj.com/2007/11/17/setting-a-custom-label-for-a-button-with-stock-icon-in-pygtk/
	 */

	box = gtk_hbox_new( FALSE, 0 );

	ctl_buttons = gtk_hbox_new( TRUE, 0 );
	gtk_box_pack_start( GTK_BOX( box ), ctl_buttons,
			FALSE, FALSE, 0 );
	gtk_widget_show( ctl_buttons );

	ctl_prev = gtk_button_new_with_label( "<<" );
	gtk_signal_connect( GTK_OBJECT( ctl_prev ), "clicked",
			G_CALLBACK( ctl_prev__click ), NULL);
	gtk_box_pack_start( GTK_BOX( ctl_buttons ), ctl_prev,
			FALSE, TRUE, 0);
	gtk_widget_show( ctl_prev );

	ctl_stop = gtk_button_new_with_label( "[]" );
	gtk_signal_connect( GTK_OBJECT( ctl_stop ), "clicked",
			G_CALLBACK( ctl_stop__click ), NULL);
	gtk_box_pack_start( GTK_BOX( ctl_buttons ), ctl_stop,
			FALSE, TRUE, 0);
	gtk_widget_show( ctl_stop );

	ctl_pause = gtk_button_new_with_label( "\"" );
	gtk_signal_connect( GTK_OBJECT( ctl_pause ), "clicked",
			G_CALLBACK( ctl_pause__click ), NULL);
	gtk_box_pack_start( GTK_BOX( ctl_buttons ), ctl_pause,
			FALSE, TRUE, 0);
	gtk_widget_show( ctl_pause );

	ctl_play = gtk_button_new_with_label( ">" );
	gtk_signal_connect( GTK_OBJECT( ctl_play ), "clicked",
			G_CALLBACK( ctl_play__click ), NULL);
	gtk_box_pack_start( GTK_BOX( ctl_buttons ), ctl_play,
			FALSE, TRUE, 0);
	gtk_widget_show( ctl_play );

	ctl_next = gtk_button_new_with_label( ">>" );
	gtk_signal_connect( GTK_OBJECT( ctl_next ), "clicked",
			G_CALLBACK( ctl_next__click ), NULL);
	gtk_box_pack_start( GTK_BOX( ctl_buttons ), ctl_next,
			FALSE, TRUE, 0);
	gtk_widget_show( ctl_next );

	return box;
}

GtkWidget *playbox_new( void )
{
	GtkWidget *ctl_rowa, *ctl_buttons;
	GtkWidget *box;

	box = gtk_vbox_new( FALSE, 0 );

	/* track details */

	/* TODO: maybe use GtkLabel instead of GtkEntry
	 * _set_selctable( TRUE ); 
	 * update of text-fields may cause window enlargment
	 */
	ctl_title = gtk_entry_new();
	gtk_entry_set_alignment( GTK_ENTRY(ctl_title), 0 );
	gtk_editable_set_editable( GTK_EDITABLE(ctl_title), FALSE );
	gtk_box_pack_start_defaults( GTK_BOX( box ), ctl_title );
	gtk_widget_show( ctl_title );

	ctl_rowa = detail2_new();
	gtk_container_add( GTK_CONTAINER( box ), ctl_rowa );
	gtk_widget_show( ctl_rowa );

	/* TODO: more details 
	 * IDs, Tags, filter, album_artist */

	/* TODO: use fill-level instead of value for progress - req. gtk 2.12*/
	ctl_prog = gtk_hscale_new_with_range(0, 1, 1);
	gtk_scale_set_value_pos( GTK_SCALE(ctl_prog), GTK_POS_LEFT );
	gtk_range_set_update_policy( GTK_RANGE(ctl_prog), GTK_UPDATE_DISCONTINUOUS );
	gtk_signal_connect( GTK_OBJECT( ctl_prog ), "format-value",
			G_CALLBACK( ctl_prog__format ), NULL);
	gtk_signal_connect( GTK_OBJECT( ctl_prog ), "value-changed",
			G_CALLBACK( ctl_prog__changed ), NULL);
	gtk_box_pack_start_defaults( GTK_BOX( box ), ctl_prog );
	gtk_widget_show( ctl_prog );

	/* buttons */
	ctl_buttons = playbox_buttons_new();
	gtk_box_pack_start( GTK_BOX( box ), ctl_buttons,
			TRUE, FALSE, 0 );
	gtk_widget_show( ctl_buttons );
	
	return(box);
}
