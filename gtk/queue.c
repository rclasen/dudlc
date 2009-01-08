/*
 * Copyright (c) 2008 Rainer Clasen
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

/* list queue */

#include "common.h"

/*
 * helper for selection processing
 */

static void queue_list_each_deltrack( 
	GtkTreeModel	*model,
	GtkTreePath	*path,
	GtkTreeIter	*iter,
	GList		**idlist)
{
	gint queueid;

	(void)path;
	gtk_tree_model_get (model, iter, QUEUELIST_ID, &queueid, -1);
	*idlist = g_list_append(*idlist, (gpointer)queueid );
}

/*
 * selection processing
 */

void queue_list_select_deltrack( GtkTreeView *list )
{
	GList *idlist = NULL, *node = NULL;

	gtk_tree_selection_selected_foreach(
		gtk_tree_view_get_selection(list),
		(GtkTreeSelectionForeachFunc)queue_list_each_deltrack, &idlist );

	for( node = idlist; node != NULL; node = node->next ){
		duc_cmd_queuedel( con, (gint)node->data );
	}
	g_list_free(idlist);

}

/*
 * context menu
 */
static void queue_list_context_deltrack_on_activate(GtkWidget *menuitem, gpointer data)
{
	(void)menuitem;
	queue_list_select_deltrack(GTK_TREE_VIEW(data));
}

static gint queue_list_context_populate( GtkWidget *view, GtkWidget *menu )
{
	gint selected;
	GtkWidget *menuitem;
	gint numitems = 0;


	if( 0 >= (selected = tree_view_select_count(GTK_TREE_VIEW(view))))
		return 0;


	menuitem = gtk_menu_item_new_with_label( "unqueue" );
	g_signal_connect(menuitem, "activate",
		(GCallback)queue_list_context_deltrack_on_activate, view);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);
	numitems++;

	if( selected == 1 ){
		/* TODO: context unqueue artist */
		/* TODO: context unqueue album */
		/* TODO: context unqueue user */

		/* TODO: context list artist albums*/
		/* TODO: context list artist tracks*/
		/* TODO: context list albums tracks */

		/* TODO: context filter artist */
		/* TODO: context filter album */
	}

	return numitems;
}

/*
 * the list view
 */

GtkWidget *queue_list_new( void )
{
	GtkTreeView *view;
	GtkTreeModel *store;
	GtkTreeViewColumn *col;
	GtkCellRenderer *renderer;

	view = GTK_TREE_VIEW(gtk_tree_view_new());

	/* store */

	store = GTK_TREE_MODEL(gtk_list_store_new( QUEUELIST_N_COLUMNS,
		G_TYPE_INT,	/* QUEUELIST_ID */
		G_TYPE_INT,	/* QUEUELIST_USER_ID */
		G_TYPE_STRING,	/* QUEUELIST_USER */
		G_TYPE_INT,	/* QUEUELIST_QUEUED */
		G_TYPE_INT,	/* QUEUELIST_ALBUM_ID */
		G_TYPE_INT,	/* QUEUELIST_ALBUM_POS */
		G_TYPE_STRING,	/* QUEUELIST_ALBUM */
		G_TYPE_INT,	/* QUEUELIST_DURATION */
		G_TYPE_INT,	/* QUEUELIST_ARTIST_ID */
		G_TYPE_STRING,	/* QUEUELIST_ARTIST */
		G_TYPE_STRING	/* QUEUELIST_TITLE */
		) );

	gtk_tree_sortable_set_sort_func( GTK_TREE_SORTABLE(store),
		QUEUELIST_ID, sortfunc_int, (gpointer)QUEUELIST_ID, NULL);

	gtk_tree_sortable_set_sort_func( GTK_TREE_SORTABLE(store),
		QUEUELIST_USER_ID, sortfunc_int, (gpointer)QUEUELIST_USER_ID, NULL);

	gtk_tree_sortable_set_sort_func( GTK_TREE_SORTABLE(store),
		QUEUELIST_USER, sortfunc_str, (gpointer)QUEUELIST_USER, NULL);

	gtk_tree_sortable_set_sort_func( GTK_TREE_SORTABLE(store),
		QUEUELIST_QUEUED, sortfunc_int, (gpointer)QUEUELIST_QUEUED, NULL);

	gtk_tree_sortable_set_sort_func( GTK_TREE_SORTABLE(store),
		QUEUELIST_ALBUM_ID, sortfunc_int, (gpointer)QUEUELIST_ALBUM_ID, NULL);

	gtk_tree_sortable_set_sort_func( GTK_TREE_SORTABLE(store),
		QUEUELIST_ALBUM_POS, sortfunc_albumpos, (gpointer)QUEUELIST_ALBUM_POS, NULL);

	gtk_tree_sortable_set_sort_func( GTK_TREE_SORTABLE(store),
		QUEUELIST_ALBUM, sortfunc_str, (gpointer)QUEUELIST_ALBUM, NULL);

	gtk_tree_sortable_set_sort_func( GTK_TREE_SORTABLE(store),
		QUEUELIST_DURATION, sortfunc_int, (gpointer)QUEUELIST_DURATION, NULL);

	gtk_tree_sortable_set_sort_func( GTK_TREE_SORTABLE(store),
		QUEUELIST_ARTIST_ID, sortfunc_int, (gpointer)QUEUELIST_ARTIST_ID, NULL);

	gtk_tree_sortable_set_sort_func( GTK_TREE_SORTABLE(store),
		QUEUELIST_ARTIST, sortfunc_str, (gpointer)QUEUELIST_ARTIST, NULL);

	gtk_tree_sortable_set_sort_func( GTK_TREE_SORTABLE(store),
		QUEUELIST_TITLE, sortfunc_str, (gpointer)QUEUELIST_TITLE, NULL);


	gtk_tree_view_set_model( GTK_TREE_VIEW(view), GTK_TREE_MODEL(store) );
	g_object_unref(store);


	/* context menu */

	context_add( view, queue_list_context_populate );


	/* columns */

	/* TODO: gtk_tree_view_column_set_resizable () */
	renderer = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new_with_attributes(
			"Id", renderer, "text", QUEUELIST_ID, NULL );
	g_object_set_data( G_OBJECT(col), "columnum", (gpointer)QUEUELIST_ID);
	g_signal_connect(col, "clicked", (GCallback)tree_view_column_on_clicked, view );
	gtk_tree_view_column_set_sort_column_id(col, QUEUELIST_ID);
	gtk_tree_view_append_column( view, col );

	renderer = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new_with_attributes(
			"User", renderer, "text", QUEUELIST_USER, NULL );
	g_object_set_data( G_OBJECT(col), "columnum", (gpointer)QUEUELIST_USER);
	g_signal_connect(col, "clicked", (GCallback)tree_view_column_on_clicked, view );
	gtk_tree_view_column_set_sort_column_id(col, QUEUELIST_USER);
	gtk_tree_view_append_column( view, col );

	renderer = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new();
	g_object_set_data( G_OBJECT(col), "columnum", (gpointer)QUEUELIST_QUEUED);
	g_signal_connect(col, "clicked", (GCallback)tree_view_column_on_clicked, view );
	gtk_tree_view_column_set_title( col, "Queued" );
	gtk_tree_view_column_pack_start( col, renderer, TRUE );
	gtk_tree_view_column_set_cell_data_func( col, renderer, 
		cellfunc_time, (gpointer)QUEUELIST_QUEUED, NULL );
	gtk_tree_view_column_set_sort_column_id(col, QUEUELIST_QUEUED);
	gtk_tree_view_append_column( view, col );

	renderer = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new();
	g_object_set_data( G_OBJECT(col), "columnum", (gpointer)QUEUELIST_ALBUM_POS);
	g_signal_connect(col, "clicked", (GCallback)tree_view_column_on_clicked, view );
	gtk_tree_view_column_set_title( col, "TrackId" );
	gtk_tree_view_column_pack_start( col, renderer, TRUE );
	gtk_tree_view_column_set_cell_data_func( col, renderer, 
		cellfunc_trackid, (gpointer)QUEUELIST_ALBUM_POS, NULL );
	gtk_tree_view_column_set_sort_column_id(col, QUEUELIST_ALBUM_POS);
	gtk_tree_view_append_column( view, col );
	
	renderer = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new();
	g_object_set_data( G_OBJECT(col), "columnum", (gpointer)QUEUELIST_DURATION);
	g_signal_connect(col, "clicked", (GCallback)tree_view_column_on_clicked, view );
	gtk_tree_view_column_set_title( col, "Duration" );
	gtk_tree_view_column_pack_start( col, renderer, TRUE );
	gtk_tree_view_column_set_cell_data_func( col, renderer, 
		cellfunc_duration, (gpointer)QUEUELIST_DURATION, NULL );
	gtk_tree_view_column_set_sort_column_id(col, QUEUELIST_DURATION);
	gtk_tree_view_append_column( view, col );

	renderer = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new_with_attributes(
			"Artist", renderer, "text", QUEUELIST_ARTIST, NULL );
	g_object_set_data( G_OBJECT(col), "columnum", (gpointer)QUEUELIST_ARTIST);
	g_signal_connect(col, "clicked", (GCallback)tree_view_column_on_clicked, view );
	gtk_tree_view_column_set_sort_column_id(col, QUEUELIST_ARTIST);
	gtk_tree_view_append_column( view, col );

	renderer = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new_with_attributes(
			"Album", renderer, "text", QUEUELIST_ALBUM, NULL );
	g_object_set_data( G_OBJECT(col), "columnum", (gpointer)QUEUELIST_ALBUM);
	g_signal_connect(col, "clicked", (GCallback)tree_view_column_on_clicked, view );
	gtk_tree_view_column_set_sort_column_id(col, QUEUELIST_ALBUM);
	gtk_tree_view_append_column( view, col );
	
	renderer = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new_with_attributes(
			"Title", renderer, "text", QUEUELIST_TITLE, NULL );
	g_object_set_data( G_OBJECT(col), "columnum", (gpointer)QUEUELIST_TITLE);
	g_signal_connect(col, "clicked", (GCallback)tree_view_column_on_clicked, view );
	gtk_tree_view_column_set_sort_column_id(col, QUEUELIST_TITLE);
	gtk_tree_view_append_column( view, col );
	
	/* TODO: add column with tags of track */
	
	gtk_tree_view_set_search_column( GTK_TREE_VIEW(view), QUEUELIST_ID );
	gtk_widget_show(GTK_WIDGET(view));

	return GTK_WIDGET(view);
}

void queue_list_add( GtkTreeModel *store, duc_queue *queue )
{
	gchar *album, *title, *artist, *user;
	GtkTreeIter add;

	album = g_locale_to_utf8( queue->track->album->album, -1, NULL, NULL, NULL);
	title = g_locale_to_utf8( queue->track->title, -1, NULL, NULL, NULL);
	artist = g_locale_to_utf8( queue->track->artist->artist, -1, NULL, NULL, NULL);
	user = g_locale_to_utf8( queue->user->name, -1, NULL, NULL, NULL);

	gtk_list_store_append( GTK_LIST_STORE(store), &add );
	gtk_list_store_set( GTK_LIST_STORE(store), &add, 
		QUEUELIST_ID, queue->id,
		QUEUELIST_USER_ID, queue->user->id,
		QUEUELIST_USER, user,
		QUEUELIST_QUEUED, queue->queued,
		QUEUELIST_ALBUM_ID, queue->track->album->id,
		QUEUELIST_ALBUM_POS, queue->track->albumnr,
		QUEUELIST_ALBUM, album,
		QUEUELIST_DURATION, queue->track->duration,
		QUEUELIST_ARTIST_ID, queue->track->artist->id,
		QUEUELIST_ARTIST, artist,
		QUEUELIST_TITLE, title,
		-1);

	g_free(album);
	g_free(title);
	g_free(artist);
	g_free(user);
}

void queue_list_populate( GtkTreeModel *store, duc_it_queue *in )
{
	duc_queue *queue;

	/* TODO unbind store from view,
	 * disable sorting: gtk_tree_sortable_set_sort_column_id */
	for( queue = duc_it_queue_cur(in); queue; queue = duc_it_queue_next(in)){
		queue_list_add( store, queue );
		duc_queue_free(queue);
	}
	/* TODO rebind store to view */
}


void queue_list_del( GtkTreeModel *store, int queueid )
{
	GtkTreeIter iter;

	if( ! gtk_tree_model_get_iter_first( GTK_TREE_MODEL(store), &iter ) )
		return;

	do {
		gint id;
		gtk_tree_model_get( store, &iter, QUEUELIST_ID, &id, -1);
		if( id == queueid ){
			gtk_list_store_remove( GTK_LIST_STORE(store), &iter );
			break;
		}
		
	} while( gtk_tree_model_iter_next( GTK_TREE_MODEL(store), &iter ));
}

void queue_list_clear( GtkTreeModel *store )
{
	gtk_list_store_clear( GTK_LIST_STORE(store) );
}

