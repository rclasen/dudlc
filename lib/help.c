#include <stdlib.h>
#include <string.h>

#include "dudlc/proto.h"
#include "dudlc/help.h"

static char *_msc_help_parse( const char *line, char **end )
{
	if( end ) (const char*) *end = line + strlen(line);
	return strdup(line);
}

msc_it_help *msc_cmd_help( mservclient *c )
{
	return _msc_iterate(c, (_msc_converter)_msc_help_parse, "help");
}

