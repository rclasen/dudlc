/*
 * Copyright (c) 2008 Rainer Clasen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

/* list albums (albumsearch, albumsartist) */

#include "common.h"

/*
 * helper for selection processinig
 */

static void album_list_each_queuealbum(
	GtkTreeModel  *model,
	GtkTreePath   *path,
	GtkTreeIter   *iter,
	gpointer       data)
{
	gint albumid;

	(void)path;
	(void)data;
	gtk_tree_model_get (model, iter, ALBUMLIST_ID, &albumid, -1);
	duc_cmd_queuealbum( con, albumid );
}

/*
 * selection processing
 */

void album_list_select( GtkTreeView *view, int selid )
{
	tree_view_select( view, ALBUMLIST_ID, selid );
}

void album_list_select_queuealbum( GtkTreeView *list )
{
	gtk_tree_selection_selected_foreach(
		gtk_tree_view_get_selection(GTK_TREE_VIEW(list)),
		album_list_each_queuealbum, NULL);
}

/*
 * the list view
 */

GtkWidget *album_list_new( gboolean full )
{
	GtkTreeView *view;
	GtkTreeViewColumn *col;
	GtkCellRenderer *renderer;

	view = GTK_TREE_VIEW(gtk_tree_view_new());


	renderer = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new_with_attributes(
			"ID", renderer, "text", ALBUMLIST_ID, NULL );
	g_object_set_data( G_OBJECT(col), "columnum", (gpointer)ALBUMLIST_ID);
	g_signal_connect(col, "clicked", (GCallback)tree_view_column_on_clicked, view );
	gtk_tree_view_column_set_resizable( col, TRUE );
	gtk_tree_view_column_set_sort_column_id(col, ALBUMLIST_ID);
	gtk_tree_view_append_column( view, col );

	renderer = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new_with_attributes(
			"Year", renderer, "text", ALBUMLIST_YEAR, NULL );
	g_object_set_data( G_OBJECT(col), "columnum", (gpointer)ALBUMLIST_YEAR);
	g_signal_connect(col, "clicked", (GCallback)tree_view_column_on_clicked, view );
	gtk_tree_view_column_set_resizable( col, TRUE );
	gtk_tree_view_column_set_sort_column_id(col, ALBUMLIST_YEAR);
	gtk_tree_view_append_column( view, col );

	if( full ){

	renderer = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new_with_attributes(
			"Artist", renderer, "text", ALBUMLIST_ARTIST, NULL );
	g_object_set_data( G_OBJECT(col), "columnum", (gpointer)ALBUMLIST_ARTIST);
	g_signal_connect(col, "clicked", (GCallback)tree_view_column_on_clicked, view );
	gtk_tree_view_column_set_resizable( col, TRUE );
	gtk_tree_view_column_set_sort_column_id(col, ALBUMLIST_ARTIST);
	gtk_tree_view_append_column( view, col );

	}

	renderer = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new_with_attributes(
			"Album", renderer, "text", ALBUMLIST_ALBUM, NULL );
	g_object_set_data( G_OBJECT(col), "columnum", (gpointer)ALBUMLIST_ALBUM);
	g_signal_connect(col, "clicked", (GCallback)tree_view_column_on_clicked, view );
	gtk_tree_view_column_set_resizable( col, TRUE );
	gtk_tree_view_column_set_sort_column_id(col, ALBUMLIST_ALBUM);
	gtk_tree_view_append_column( view, col );


	gtk_tree_view_set_search_column( GTK_TREE_VIEW(view), ALBUMLIST_ARTIST );
	gtk_widget_show(GTK_WIDGET(view));

	return GTK_WIDGET(view);
}

GtkWidget *album_list_new_with_list( gboolean full, duc_it_album *in )
{
	GtkWidget *view;

	if( NULL == (view = album_list_new(full)))
		return NULL;

	album_list_populate( GTK_TREE_VIEW(view), in );

	return view;
}

void album_list_populate( GtkTreeView *view, duc_it_album *in )
{
	GtkTreeModel *store;
	GtkTreeIter add;
	duc_album *album;

	store = GTK_TREE_MODEL(gtk_list_store_new( ALBUMLIST_N_COLUMNS,
		G_TYPE_INT,
		G_TYPE_INT,
		G_TYPE_INT,
		G_TYPE_STRING,
		G_TYPE_STRING ) );

	for( album = duc_it_album_cur(in); album; album = duc_it_album_next(in)){
		gchar *salbum, *sartist;

		salbum = g_locale_to_utf8( album->album, -1, NULL, NULL, NULL);
		sartist = g_locale_to_utf8( album->artist->artist, -1, NULL, NULL, NULL);

		gtk_list_store_append( GTK_LIST_STORE(store), &add );
		gtk_list_store_set( GTK_LIST_STORE(store), &add,
			ALBUMLIST_ID, album->id,
			ALBUMLIST_YEAR, album->year,
			ALBUMLIST_ARTIST_ID, album->artist->id,
			ALBUMLIST_ARTIST, sartist,
			ALBUMLIST_ALBUM, salbum,
			-1);

		g_free(salbum);
		g_free(sartist);
		duc_album_free( album );
	}

	gtk_tree_sortable_set_sort_func( GTK_TREE_SORTABLE(store),
		ALBUMLIST_ID, sortfunc_int, (gpointer)ALBUMLIST_ID, NULL);

	gtk_tree_sortable_set_sort_func( GTK_TREE_SORTABLE(store),
		ALBUMLIST_YEAR, sortfunc_int, (gpointer)ALBUMLIST_YEAR, NULL);

	gtk_tree_sortable_set_sort_func( GTK_TREE_SORTABLE(store),
		ALBUMLIST_ARTIST_ID, sortfunc_int, (gpointer)ALBUMLIST_ARTIST_ID, NULL);

	gtk_tree_sortable_set_sort_func( GTK_TREE_SORTABLE(store),
		ALBUMLIST_ARTIST, sortfunc_str, (gpointer)ALBUMLIST_ARTIST, NULL);

	gtk_tree_sortable_set_sort_func( GTK_TREE_SORTABLE(store),
		ALBUMLIST_ALBUM, sortfunc_str, (gpointer)ALBUMLIST_ALBUM, NULL);

	gtk_tree_view_set_model( GTK_TREE_VIEW(view), GTK_TREE_MODEL(store) );
	g_object_unref(store);
}

void album_list_clear( GtkTreeView *view )
{
	GtkTreeModel *store;

	store = gtk_tree_view_get_model( view );
	gtk_list_store_clear( GTK_LIST_STORE(store) );
}


