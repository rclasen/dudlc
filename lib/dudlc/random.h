/*
 * Copyright (c) 2008 Rainer Clasen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

#ifndef _DUDLC_RANDOM_H
#define _DUDLC_RANDOM_H

#include "session.h"
#include "track.h"

char *duc_cmd_filter( dudlc *c );
int duc_cmd_filterset( dudlc *c, const char *filter );
int duc_cmd_filterstat( dudlc *c );

duc_it_track *duc_cmd_filtertop( dudlc *c, int num );

void _duc_bcast_random( dudlc *c, const char *line );

#endif
