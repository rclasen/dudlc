/*
 * Copyright (c) 2008 Rainer Clasen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

// #include <stdlib.h>
#include <stdio.h>
#include <glib.h>

#include "common.h"

static void def_string( char **dst, GKeyFile *kf, char *key, char *def )
{
	GError *err = NULL;

	if( *dst )
		return;

	if( kf )
		*dst = g_key_file_get_string( kf, "dudlc", key, &err );

	if( ! *dst )
		*dst = def;
}

static void def_integer( int *dst, GKeyFile *kf, char *key, int def )
{
	GError *err = NULL;

	if( *dst >= 0 )
		return;

	if( kf )
		*dst = g_key_file_get_integer( kf, "dudlc", key, &err );
	if( err && err->code == G_KEY_FILE_ERROR_INVALID_VALUE )
		fprintf( stderr, "invalid data for %s: %s", key, err->message );

	if( err || *dst < 0 )
		*dst = def;
}

void options_init( options *opt )
{
	opt->host = NULL;
	opt->port = -1;
	opt->user = NULL;
	opt->pass = NULL;
}

int options_read( options *opt, const char *fname )
{
	GKeyFile *keyfile = NULL;
	GError *err = NULL;

	keyfile = g_key_file_new();
	if( ! g_key_file_load_from_file( keyfile, fname, G_KEY_FILE_NONE, &err )){
		if( err->domain == G_KEY_FILE_ERROR &&
				err->code == G_KEY_FILE_ERROR_NOT_FOUND )
			fprintf( stderr, "error reading config %s: %s",
					fname, err->message );
		g_key_file_free( keyfile );
		keyfile = NULL;
	}

	def_string( &opt->host, keyfile, "host", "localhost" );
	def_integer( &opt->port, keyfile, "port", 4445 );
	def_string( &opt->user, keyfile, "user", "guest" );
	def_string( &opt->pass, keyfile, "pass", "guest" );

	if( keyfile )
		g_key_file_free( keyfile );

	return 0;
}

