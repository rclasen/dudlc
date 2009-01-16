/*
 * Copyright (c) 2008 Rainer Clasen
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

/* list artists (artistsearch, artistsartist) */

#include "common.h"

void artist_list_select( GtkTreeView *view, int selid )
{
	tree_view_select( view, ALBUMLIST_ID, selid );
}

/*
 * the list view
 */

GtkWidget *artist_list_new( void )
{
	GtkTreeView *view;
	GtkTreeViewColumn *col;
	GtkCellRenderer *renderer;

	view = GTK_TREE_VIEW(gtk_tree_view_new());

	renderer = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new_with_attributes(
			"ID", renderer, "text", ARTISTLIST_ID, NULL );
	g_object_set_data( G_OBJECT(col), "columnum", (gpointer)ARTISTLIST_ID);
	g_signal_connect(col, "clicked", (GCallback)tree_view_column_on_clicked, view );
	gtk_tree_view_column_set_resizable( col, TRUE );
	gtk_tree_view_column_set_sort_column_id(col, ARTISTLIST_ID);
	gtk_tree_view_append_column( view, col );

	renderer = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new_with_attributes(
			"Artist", renderer, "text", ARTISTLIST_ARTIST, NULL );
	g_object_set_data( G_OBJECT(col), "columnum", (gpointer)ARTISTLIST_ARTIST);
	g_signal_connect(col, "clicked", (GCallback)tree_view_column_on_clicked, view );
	gtk_tree_view_column_set_resizable( col, TRUE );
	gtk_tree_view_column_set_sort_column_id(col, ARTISTLIST_ARTIST);
	gtk_tree_view_append_column( view, col );
	
	
	gtk_tree_view_set_search_column( GTK_TREE_VIEW(view), ARTISTLIST_ARTIST );
	gtk_widget_show(GTK_WIDGET(view));

	return GTK_WIDGET(view);
}

GtkWidget *artist_list_new_with_list( duc_it_artist *in )
{
	GtkWidget *view;

	if( NULL == (view = artist_list_new())) 
		return NULL;

	artist_list_populate( GTK_TREE_VIEW(view), in );

	return view;
}

void artist_list_populate( GtkTreeView *view, duc_it_artist *in )
{
	GtkTreeModel *store;
	GtkTreeIter add;
	duc_artist *artist;

	store = GTK_TREE_MODEL(gtk_list_store_new( ARTISTLIST_N_COLUMNS,
		G_TYPE_INT,
		G_TYPE_STRING ) );

	for( artist = duc_it_artist_cur(in); artist; artist = duc_it_artist_next(in)){
		gchar *sartist;

		sartist = g_locale_to_utf8( artist->artist, -1, NULL, NULL, NULL);

		gtk_list_store_append( GTK_LIST_STORE(store), &add );
		gtk_list_store_set( GTK_LIST_STORE(store), &add, 
			ARTISTLIST_ID, artist->id,
			ARTISTLIST_ARTIST, sartist,
			-1);

		g_free(sartist);
		duc_artist_free( artist );
	}

	gtk_tree_sortable_set_sort_func( GTK_TREE_SORTABLE(store),
		ARTISTLIST_ID, sortfunc_int, (gpointer)ARTISTLIST_ID, NULL);

	gtk_tree_sortable_set_sort_func( GTK_TREE_SORTABLE(store),
		ARTISTLIST_ARTIST, sortfunc_str, (gpointer)ARTISTLIST_ARTIST, NULL);

	gtk_tree_view_set_model( GTK_TREE_VIEW(view), GTK_TREE_MODEL(store) );
	g_object_unref(store);
}

