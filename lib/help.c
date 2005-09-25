#include <stdlib.h>
#include <string.h>

#include "dudlc/proto.h"
#include "dudlc/help.h"

static char *_duc_help_parse( char *line, char **end )
{
	if( end ) *end = line + strlen(line);
	return strdup(line);
}

duc_it_help *duc_cmd_help( dudlc *c )
{
	return _duc_iterate(c, (_duc_converter)_duc_help_parse, "help");
}

