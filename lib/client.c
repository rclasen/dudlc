#include <stdlib.h>

#include "dudlc/proto.h"
#include "dudlc/event.h"
#include "dudlc/client.h"

static duc_client *_duc_client_parse( const char *line, char **end )
{
	duc_client *c;

	const char *s;
	char *e;

	(const char*)e = s = line;

	if( NULL == (c = malloc(sizeof(duc_client))))
		goto clean1;

	c->id = strtol(s, &e, 10 );
	if( s == e )
		goto clean2;

	s = e+1;
	if( NULL == (c->addr = _duc_fielddup( s, &e )))
		goto clean2;
	
	s = e+1;
	if( NULL == (c->user = _duc_user_parse(s, &e )))
		goto clean3;

	if( end ) *end = e;
	return c;

clean3:
	free(c->addr);
clean2:
	free(c);
clean1:
	if( end ) (const char *)*end = line;
	return NULL;
}

void duc_client_free( duc_client *c )
{
	if( ! c )
		return;

	duc_user_free( c->user );
	free( c->addr );
	free(c);
}

duc_it_client *duc_cmd_clientlist( dudlc *c )
{
	return _duc_iterate( c, (_duc_converter)_duc_client_parse, 
			"clientlist" );
}

int duc_cmd_clientclose( dudlc *c, int id )
{
	return _duc_cmd_succ(c, "clientclose %u", id );
}

int duc_cmd_clientcloseuser( dudlc *c, int uid )
{
	return _duc_cmd_succ(c, "clientcloseuser %d", uid );
}

void _duc_bcast_client( dudlc *c, const char *line )
{
	duc_client *u;

	switch(line[2]){
		case '0': /* login */
			if( NULL != (u = _duc_client_parse( line+4, NULL ))){
				_MSC_EVENT(c,login,c,u);
				duc_client_free(u);
			}
			break;

		case '1': /* logout */
			if( NULL != (u = _duc_client_parse( line+4, NULL ))){
				_MSC_EVENT(c,logout,c,u);
				duc_client_free(u);
			}
			break;

		case '2': /* I was kicked */
			_MSC_EVENT(c,kicked,c);
			break;

		default:
			_MSC_EVENT(c,bcast,c,line);
	}
	(void)c;
}
