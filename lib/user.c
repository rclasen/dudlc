#include <stdlib.h>

#include "dudlc/proto.h"
#include "dudlc/event.h"
#include "dudlc/user.h"

duc_user *_duc_user_parse( const char *line, char **end )
{
	duc_user *c;

	const char *s;
	char *e;

	(const char*)e = s = line;

	if( NULL == (c = malloc(sizeof(duc_user))))
		goto clean1;

	c->id = strtol(s, &e, 10 );
	if( s == e )
		goto clean2;

	s = e+1;
	if( NULL == (c->name = _duc_fielddup( s, &e )))
		goto clean2;
		s = e+1;

	c->right = strtol(s, &e, 10 );
	if( s == e )
		goto clean3;


	if( end ) *end = e;
	return c;
clean3:
	free(c->name);
clean2:
	free(c);
clean1:
	if( end ) (const char *)*end = line;
	return NULL;
}

void duc_user_free( duc_user *c )
{
	if( ! c )
		return;

	free( c->name);
	free(c);
}



duc_it_user *duc_cmd_userlist( dudlc *c )
{
	return _duc_iterate( c, (_duc_converter)_duc_user_parse, 
			"userlist" );
}

duc_user *duc_cmd_userget( dudlc *c, int uid )
{
	return _duc_cmd_conv(c, (_duc_converter)_duc_user_parse, 
			"userget %d", uid);
}

int duc_cmd_usergetname( dudlc *c, const char *name )
{
	return _duc_cmd_int(c, "user2id %s", name);
}

int duc_cmd_usersetpass( dudlc *c, int uid, const char *pass )
{
	return _duc_cmd_succ(c, "usersetpass %d %s", uid, pass);
}

int duc_cmd_usersetright( dudlc *c, int uid, int right )
{
	return _duc_cmd_succ(c, "usersetright %d %d", uid, right);
}

int duc_cmd_userdel( dudlc *c, int uid )
{
	return _duc_cmd_succ(c, "userdel %d", uid);
}

int duc_cmd_useradd( dudlc *c, const char *name )
{
	return _duc_cmd_int(c, "useradd %s", name);
}





