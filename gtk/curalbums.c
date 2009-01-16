/*
 * Copyright (c) 2008 Rainer Clasen
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

#include "common.h"

static GtkWidget *albums_view;

GtkWidget *curalbums_new( GtkAction *action )
{
	GtkWidget *win;
	GtkTreeSelection *sel;

	albums_view = album_list_new(TRUE);
	sel = gtk_tree_view_get_selection( GTK_TREE_VIEW(albums_view));
	gtk_tree_selection_set_mode( GTK_TREE_SELECTION(sel), GTK_SELECTION_MULTIPLE );

	win = childscroll_new( "albums of current artist", albums_view, NULL, NULL );
	gtk_signal_connect( GTK_OBJECT(win), "delete_event", 
			G_CALLBACK( toggle_win_on_delete ), action);
	gtk_window_set_default_size(GTK_WINDOW(win), 500, 300);

	return win;
}

void curalbums_populate( duc_it_album *it )
{
	album_list_populate( GTK_TREE_VIEW(albums_view), it );
}

void curalbums_clear( void )
{
	album_list_clear( GTK_TREE_VIEW(albums_view) );
}


