/*
 * Copyright (c) 2008 Rainer Clasen
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms described in the file LICENSE included in this
 * distribution.
 *
 */

#ifndef _PLAYBOX_H
#define _PLAYBOX_H

GtkWidget *playbox_new( dudlc *con );
void playbox_buttons_show( gboolean show );
void playbox_buttons_enable( duc_playstatus stat );
void playbox_detail_update( duc_track *t );
void playbox_detail_clear( void );
void playbox_progress_update( int r );

#endif //  _PLAYBOX_H
