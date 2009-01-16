/*
 * Copyright (c) 2008 Rainer Clasen
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

#include "common.h"

/*
 * helper for selection processinig
 */

static void tree_view_each_count(
	GtkTreeModel  *model,
	GtkTreePath   *path,
	GtkTreeIter   *iter,
	gpointer       data)
{
	(void)model;
	(void)path;
	(void)iter;
	++(*(gint*)data);
}

/*
 * selection processing
 */

void tree_view_select( GtkTreeView *view, int col, int selid )
{
	GtkTreeModel *store;
	GtkTreeSelection *sel;
	GtkTreeIter iter;
	GtkTreePath *path;

	store = gtk_tree_view_get_model( view );
	sel = gtk_tree_view_get_selection( view );

	if( ! gtk_tree_model_get_iter_first( GTK_TREE_MODEL(store), &iter ) )
		return;

	do {
		gint modid;
		gtk_tree_model_get( store, &iter, col, &modid, -1);
		if( selid == modid ){
			gtk_tree_selection_select_iter( sel, &iter );
			path = gtk_tree_model_get_path( store, &iter );
			break;
		}
		
	} while( gtk_tree_model_iter_next( GTK_TREE_MODEL(store), &iter ));

	if( path )
		gtk_tree_view_scroll_to_cell( view, path, NULL, FALSE, 0, 0);
}

gint tree_view_select_count( GtkTreeView *list )
{
	gint selected = 0;
	gtk_tree_selection_selected_foreach(
		gtk_tree_view_get_selection(list),
		tree_view_each_count, &selected);

	return selected;
}


/*
 * helper to select inc-search column
 */

void tree_view_column_on_clicked( GtkWidget *col, GtkTreeView *view )
{
	gint id;

	id = (gint)(g_object_get_data(G_OBJECT(col), "columnum"));
	gtk_tree_view_set_search_column( GTK_TREE_VIEW(view), id );
}


