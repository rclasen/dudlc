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

GtkWidget *queuewin_new( GtkAction *action )
{
	GtkWidget *win;
	GtkTreeSelection *sel;

	queue_view = queue_list_new();
	queue_store = gtk_tree_view_get_model( GTK_TREE_VIEW(queue_view));

	sel = gtk_tree_view_get_selection( GTK_TREE_VIEW(queue_view));
	gtk_tree_selection_set_mode( GTK_TREE_SELECTION(sel), GTK_SELECTION_MULTIPLE );

	win = childscroll_new( "queue", queue_view );
	gtk_signal_connect( GTK_OBJECT(win), "delete_event", 
			G_CALLBACK( toggle_win_on_delete ), action);
	gtk_window_set_default_size(GTK_WINDOW(win), 800, 300);

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




