#ifndef _COMMAND_H
#define _COMMAND_H

typedef char *(*t_generator)( const char *text, int state );

t_generator command_completer( const char* line, unsigned int pos );
void command_action( const char *line );

#endif

