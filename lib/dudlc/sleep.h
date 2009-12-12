/*
 * Copyright (c) 2008 Rainer Clasen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

#ifndef _DUDLC_SLEEP_H
#define _DUDLC_SLEEP_H

#include "session.h"

int duc_cmd_sleep( dudlc *c );
int duc_cmd_sleepset( dudlc *c, int delay );

#endif
