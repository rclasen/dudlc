#ifndef _INPUT_H
#define _INPUT_H

#include <stdarg.h>

void tty_init( const char *name, const char *prompt );
void tty_done( void );

int tty_poll( void );

int tty_vmsg( char *fmt, va_list ap );
int tty_msg( char * fmt, ... );

#endif
