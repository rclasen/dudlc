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

