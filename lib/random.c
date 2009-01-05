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
#include "dudlc/event.h"
#include "dudlc/random.h"

char *duc_cmd_filter( dudlc *c )
{
	return _duc_cmd_string(c, "filter" );
}

int duc_cmd_filterset( dudlc *c, const char *filter )
{
	return _duc_cmd_succ(c, "filterset %s", filter );
}

int duc_cmd_filterstat( dudlc *c )
{
	return _duc_cmd_int(c, "filterstat" );
}

duc_it_track *duc_cmd_filtertop( dudlc *c, int num )
{
	return _duc_it_track_new( c, "filtertop %d", num );
}

void _duc_bcast_random( dudlc *c, const char *line )
{
	switch(line[2]){
		case '0': /* filter changed */
			_MSC_EVENT(c,filter,c,line+4);
			break;

		case '1': /* sleeo changed - misplaced 621 */
			_MSC_EVENT(c,sleep,c,atoi(line+4));
			break;


		default:
			_MSC_EVENT(c,bcast,c,line);
	}
}


