/*
 * Copyright (c) 2008 Rainer Clasen
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

#include "common.h"

gint sortfunc_str( GtkTreeModel *model,
                          GtkTreeIter  *a,
                          GtkTreeIter  *b,
                          gpointer      data)
{
	gint ret = 0;
	gchar *val1, *val2;

	gtk_tree_model_get(model, a, (gint)data, &val1, -1);
	gtk_tree_model_get(model, b, (gint)data, &val2, -1);

	if( val1 == NULL ){
		ret = ( val2 == NULL) ? 0 : -1;

	} else if( val2 == NULL ){
		ret = 1;

	} else {
		ret = g_utf8_collate(val1,val2);
	}

	g_free(val1);
	g_free(val2);
	return( ret);
}

gint sortfunc_int( GtkTreeModel *model,
                          GtkTreeIter  *a,
                          GtkTreeIter  *b,
                          gpointer      data)
{
	gint val1, val2;

	gtk_tree_model_get(model, a, (gint)data, &val1, -1);
	gtk_tree_model_get(model, b, (gint)data, &val2, -1);

	if( val1 == val2 )
		return 0;

	return ( val1 < val2 ) ? -1 : 1;
}

