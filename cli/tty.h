#ifndef _INPUT_H
#define _INPUT_H

#include <stdarg.h>

void tty_init( const char *name, const char *prompt );
void tty_done( void );

void tty_redraw( void );
int tty_poll( void );

int tty_vmsg( const char *fmt, va_list ap );
int tty_msg( const char *fmt, ... );

#endif
