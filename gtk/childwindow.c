/*
 * Copyright (c) 2008 Rainer Clasen
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

#include "common.h"

GtkWidget *childwindow_new( const char *title, GtkWidget *contents )
{
	GtkWidget *me;

	me = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	gtk_window_set_title( GTK_WINDOW(me), title );

	gtk_container_add( GTK_CONTAINER( me ), contents);
	gtk_widget_show( contents );

	/* TODO: ^W hotkey */

	return me;
}

GtkWidget *child_queue( GtkWidget *list )
{
	GtkWidget *scroll, *win;

	scroll = gtk_scrolled_window_new( GTK_ADJUSTMENT(NULL), GTK_ADJUSTMENT(NULL) );
	gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW(scroll), 
		GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
	gtk_container_add( GTK_CONTAINER(scroll), list );

	win = childwindow_new( "queue", scroll );
	gtk_window_set_default_size(GTK_WINDOW(win), 600, 300);

	return win;
}


