/*
 * Copyright (c) 2008 Rainer Clasen
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

#ifndef _DUDLC_HELP_H
#define _DUDLC_HELP_H

#include "session.h"
#include "iterate.h"

typedef char duc_help;

duc_help *_duc_help_parse( char *line, char **end );
void duc_help_free( duc_help *h );
DEFINE_DUC_ITER_PARSE(help);

duc_it_help *duc_cmd_help( dudlc *con );

#endif

