/*
 * Copyright (c) 2008 Rainer Clasen
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

#include "common.h"

void cellfunc_trackid( 
	GtkTreeViewColumn *col,
	GtkCellRenderer *cell,
	GtkTreeModel *model,
	GtkTreeIter *iter,
	gpointer data )
{
	gint albumid, albumpos;
	gchar buf[100];

	(void)col;
	gtk_tree_model_get( model, iter, (int)data -1, &albumid, -1 );
	gtk_tree_model_get( model, iter, (int)data, &albumpos, -1 );

	g_snprintf(buf, sizeof(buf), "%d/%d", albumid, albumpos );

	g_object_set(cell, "text", buf, NULL );
}

void cellfunc_duration( 
	GtkTreeViewColumn *col,
	GtkCellRenderer *cell,
	GtkTreeModel *model,
	GtkTreeIter *iter,
	gpointer data )
{
	gint duration;
	gchar buf[100];

	(void)col;
	gtk_tree_model_get( model, iter, (int)data, &duration, -1 );

	g_snprintf(buf, sizeof(buf), "%d:%02d", (gint)(duration / 60), 
		(gint)(duration % 60) );

	g_object_set(cell, "text", buf, NULL );
}


