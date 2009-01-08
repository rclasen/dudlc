/*
 * Copyright (c) 2008 Rainer Clasen
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

#include "common.h"

static void context_show( GtkWidget *view, GdkEventButton *event, gpointer data)
{
	GtkWidget *menu;

	if( NULL == data )
		return;


	menu = gtk_menu_new();
	if( 0 < (*(context_populate_func)data)( view, menu )){

		gtk_widget_show_all(menu);

		gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL,
			(event != NULL) ? event->button : 0,
			gdk_event_get_time((GdkEvent*)event));

	} else {
		gtk_widget_destroy(menu);
	}
}

static gboolean context_on_popup( GtkWidget *view, gpointer data)
{
	context_show(view, NULL, data);

	return TRUE;
}

static gboolean context_on_button( GtkWidget *view, GdkEventButton *event, gpointer data )
{
	if( event->type != GDK_BUTTON_PRESS || event->button != 3)
		return FALSE;

        if( tree_view_select_count(GTK_TREE_VIEW(view)) <= 1 ){
		GtkTreePath *path;

		if( gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(view),
                                             (gint) event->x, 
                                             (gint) event->y,
                                             &path, NULL, NULL, NULL)){

			GtkTreeSelection *selection;

			selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(view));
			gtk_tree_selection_unselect_all(selection);
			gtk_tree_selection_select_path(selection, path);
			gtk_tree_path_free(path);
		}
        }

	context_show(view, event, data);

	return TRUE;
}

void context_add( GtkTreeView *view, context_populate_func func )
{
	g_signal_connect( view, "popup-menu", (GCallback)context_on_popup, func );
	g_signal_connect( view, "button-press-event", (GCallback)context_on_button, func );
}


