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
	(*(gint*)data)++;
}

/*
 * selection processing
 */

gint tree_view_select_count( GtkTreeView *list )
{
	gint selected = 0;
	gtk_tree_selection_selected_foreach(
		gtk_tree_view_get_selection(list),
		tree_view_each_count, &selected);

	return selected;
}

/*
 *
 */

void tree_view_column_on_clicked( GtkWidget *col, GtkTreeView *view )
{
	gint id;

	id = (gint)(g_object_get_data(G_OBJECT(col), "columnum"));
	gtk_tree_view_set_search_column( GTK_TREE_VIEW(view), id );
}


