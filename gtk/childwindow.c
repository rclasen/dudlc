/*
 * Copyright (c) 2008 Rainer Clasen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

#include "common.h"

int toggle_win_on_delete( GtkWidget *widget, GdkEvent  *event, GtkAction *action )
{
	(void)widget;
	(void)event;
	gtk_toggle_action_set_active(GTK_TOGGLE_ACTION(action), FALSE);
	return TRUE;
}

static void on_add_widget( GtkUIManager *ui, GtkWidget *item, GtkWidget *box )
{
	(void)ui;

	gtk_box_pack_start( GTK_BOX( box ), item,
		FALSE, FALSE, 0 );
	gtk_widget_show( item );
}

GtkWidget *childwindow_new(
	const char *title,
	GtkWidget *contents,
	GtkUIManager *ui,
	const char *uidesc)
{
	GtkWidget *me, *mainbox;
	GError *err = NULL;

	/* window */
	me = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	gtk_window_set_title( GTK_WINDOW(me), title );

	/* mainbox */
	mainbox = gtk_vbox_new( FALSE, 0 );
	gtk_container_add( GTK_CONTAINER( me ), mainbox);
	gtk_widget_show( mainbox );

	/* ui manager */
	if( ui && uidesc ){
		GtkAccelGroup *accels;

		g_object_set_data( G_OBJECT(me), "uimanager", ui );

		g_signal_connect( G_OBJECT(ui), "add-widget",
			G_CALLBACK(on_add_widget), mainbox );

		if (!gtk_ui_manager_add_ui_from_string(ui, uidesc, -1, &err)) {
			g_message("Building UI failed : %s", err->message);
			g_error_free(err);
			g_assert(0);
			return NULL;
		}
		gtk_ui_manager_ensure_update(ui);

		/* hotkeys */
		if( NULL != (accels = gtk_ui_manager_get_accel_group( ui )))
			gtk_window_add_accel_group( GTK_WINDOW(me), accels );
	}

	gtk_container_add( GTK_CONTAINER( mainbox ), contents);
	gtk_widget_show( contents );

	return me;
}


GtkWidget *childscroll_new(
	const char *title,
	GtkWidget *contents,
	GtkUIManager *ui,
	const char *uidesc )
{
	GtkWidget *scroll;

	scroll = gtk_scrolled_window_new( GTK_ADJUSTMENT(NULL), GTK_ADJUSTMENT(NULL) );
	gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW(scroll),
		GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
	gtk_container_add( GTK_CONTAINER(scroll), contents );
	gtk_widget_show( contents );

	return childwindow_new( title, scroll, ui, uidesc );
}
