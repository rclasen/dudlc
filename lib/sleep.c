/*
 * Copyright (c) 2008 Rainer Clasen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

#include <stdlib.h>

#include "dudlc/proto.h"
#include "dudlc/sleep.h"

int duc_cmd_sleep( dudlc *c )
{
	return _duc_cmd_int(c, "sleep" );
}

int duc_cmd_sleepset( dudlc *c, int delay )
{
	return _duc_cmd_succ(c, "sleepset %d", delay );
}

