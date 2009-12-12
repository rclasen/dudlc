/*
 * Copyright (c) 2008 Rainer Clasen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

#include <stdlib.h>
#include <string.h>

#include "dudlc/proto.h"
#include "dudlc/help.h"

duc_it_help *_duc_it_help_new( dudlc *p, const char *cmd, ... )
{
	_duc_iter *it;
	va_list ap;

	va_start(ap,cmd);
	it = _duc_it_newv(p,
		(_duc_converter)_duc_help_parse,
		(_duc_free_func)duc_help_free,
		cmd, ap );
	va_end(ap);

	return it;
}

duc_help *_duc_help_parse( char *line, char **end )
{
	if( end ) *end = line + strlen(line);
	return strdup(line);
}

void duc_help_free( duc_help *h )
{
	free(h);
}

duc_it_help *duc_cmd_help( dudlc *c )
{
	return _duc_it_help_new(c, "help");
}

