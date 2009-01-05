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

/* TODO: use macro to define iterator methods */
#define duc_it_help	_duc_iter
#define duc_it_help_cur(x)	(char*)_duc_it_cur((_duc_iter*)x)
#define duc_it_help_next(x)	(char*)_duc_it_next((_duc_iter*)x)
#define duc_it_help_done(x)	_duc_it_done((_duc_iter*)x)

duc_it_help *duc_cmd_help( dudlc *con );

#endif

