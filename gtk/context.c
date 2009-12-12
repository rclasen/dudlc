/*
 * Copyright (c) 2008 Rainer Clasen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

#include "common.h"

static void context_show( GdkEventButton *event, GtkMenu *popup )
{
	g_return_if_fail(popup);

	gtk_menu_popup(GTK_MENU(popup), NULL, NULL, NULL, NULL,
		(event != NULL) ? event->button : 0,
		gdk_event_get_time((GdkEvent*)event));
}

static gboolean context_on_popup( GtkWidget *view, gpointer data)
{
	(void)view;
	context_show( NULL, data);

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

	context_show( event, data);

	return TRUE;
}

static void context_menu_unref( GtkMenu *menu, GClosure *closure )
{
	(void)closure;
	g_object_unref(G_OBJECT(menu));
}

void context_add( GtkTreeView *view, GtkMenu *menu )
{
	g_return_if_fail( menu );
	g_object_ref( G_OBJECT(menu) );
	g_signal_connect_data( view, "popup-menu", (GCallback)context_on_popup, menu,
		(GClosureNotify)context_menu_unref, 0 );
	g_signal_connect( view, "button-press-event", (GCallback)context_on_button, menu );
}


