/*
 * Copyright (c) 2008 Rainer Clasen
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

#include <string.h>

#include "common.h"

typedef struct {
	GtkWidget	*input;
	GtkWidget	*list;
	GtkWidget	*status;
	gchar		*filter;
} search_widgets;

static void search_button_on_click( GtkButton *widget, search_widgets *wg )
{
	duc_it_track *it;
	(void)widget;

	gtk_statusbar_pop( GTK_STATUSBAR(wg->status), 0 );
	g_free(wg->filter);

	wg->filter = g_strdup( gtk_entry_get_text( GTK_ENTRY(wg->input) ));

	gtk_statusbar_push( GTK_STATUSBAR(wg->status), 0,
		"searching ..." );
	// TODO: g_locale_to_utf8( wg->status, len, r, w, &err )
	it = duc_cmd_tracksearchf( con, wg->filter );
	gtk_statusbar_pop( GTK_STATUSBAR(wg->status), 0 );

	gtk_statusbar_push( GTK_STATUSBAR(wg->status), 0, (it == NULL)
		? "failed" : wg->filter );
	track_list_populate( GTK_TREE_VIEW(wg->list), it );
}

static void tracksearch_on_destroy( GtkWidget *widget, search_widgets *wg )
{
	(void)widget;
	g_free(wg->filter);
	g_free(wg);
}

GtkWidget *tracksearch_window( void )
{
	GtkWidget *win;
	GtkWidget *winbox;
	GtkWidget *search_box;
	GtkWidget *search_label;
	GtkWidget *search_button;
	GtkWidget *scroll;
	GtkTreeSelection *sel;
	search_widgets *wg;


	/* top box */
	winbox = gtk_vbox_new( FALSE, 0 );

	/* window */
	win = childwindow_new( "search tracks", winbox );
	gtk_window_set_default_size(GTK_WINDOW(win), 600, 300);

	/* TODO: use g_object_set_data() */
	wg = g_malloc(sizeof(search_widgets));
	memset(wg, 0, sizeof(search_widgets));

	gtk_signal_connect( GTK_OBJECT(win), "destroy",
		G_CALLBACK(tracksearch_on_destroy), wg);

	/* TODO: give some hint about filter syntax */

	/* input */
	search_box = gtk_hbox_new( FALSE, 0 );
	gtk_box_pack_start( GTK_BOX(winbox), search_box, FALSE, FALSE, 1 );
	gtk_widget_show( search_box );

	search_label = gtk_label_new("filter:");
	gtk_box_pack_start( GTK_BOX(search_box), search_label, FALSE, FALSE, 1 );
	gtk_widget_show( search_label );

	/* TODO: remember input, filter history */
	wg->input = gtk_entry_new();
	g_object_set( wg->input, "activates-default", TRUE, NULL );
	gtk_box_pack_start( GTK_BOX(search_box), wg->input, TRUE, TRUE, 1 );
	gtk_widget_show( wg->input );

	search_button = gtk_button_new_with_label("search");
	gtk_signal_connect( GTK_OBJECT(search_button), "clicked",
		G_CALLBACK(search_button_on_click), wg );
	gtk_box_pack_start( GTK_BOX(search_box), search_button, FALSE, FALSE, 1 );
	GTK_WIDGET_SET_FLAGS( search_button, GTK_CAN_DEFAULT);
	gtk_window_set_default( GTK_WINDOW(win), search_button );
	gtk_widget_show( search_button );



	/* scrolled result list */
	scroll = gtk_scrolled_window_new( GTK_ADJUSTMENT(NULL), GTK_ADJUSTMENT(NULL) );
	gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW(scroll), 
		GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );

	wg->list = track_list_new_with_list( TRUE, NULL );
	sel = gtk_tree_view_get_selection( GTK_TREE_VIEW(wg->list) );
	gtk_tree_selection_set_mode( GTK_TREE_SELECTION(sel), GTK_SELECTION_MULTIPLE );
	gtk_container_add( GTK_CONTAINER(scroll), wg->list );
	gtk_widget_show( wg->list );

	gtk_box_pack_start( GTK_BOX( winbox ), scroll, TRUE, TRUE, 0 );
	gtk_widget_show( scroll );

	/* status bar */
	wg->status = gtk_statusbar_new();
	gtk_box_pack_start( GTK_BOX(winbox), wg->status, FALSE, FALSE, 0 );
	gtk_widget_show( wg->status );

	return win;
}

