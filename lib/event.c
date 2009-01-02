/*
 * Copyright (c) 2008 Rainer Clasen
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */


#include "dudlc/event.h"
#include "dudlc/command.h"

duc_events *duc_getevents( dudlc *c )
{
	return c->ev;
}

duc_events *duc_setevents( dudlc *c, duc_events *e )
{
	duc_events *old;

	old = c->ev;
	c->ev = e;
	return old;
}

void _duc_bcast( dudlc *p, char *line )
{
	switch(line[1]){
		case '3':
			_duc_bcast_client( p, line );
			break;

		case '4':
			_duc_bcast_player( p, line );
			break;
			
		case '5':
			_duc_bcast_random( p, line );
			break;
			
		case '6':
			_duc_bcast_queue( p, line );
			break;
			
		case '7':
			_duc_bcast_tag( p, line );
			break;
			
		default:
			_MSC_EVENT(p,bcast,p,line);
	}
}


