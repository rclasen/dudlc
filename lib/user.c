#include <stdlib.h>

#include <mservclient/proto.h>
#include <mservclient/event.h>
#include <mservclient/user.h>

static msc_user *_msc_user_parse( const char *line, char **end )
{
	msc_user *c;

	const char *s;
	char *e;

	(const char*)e = s = line;

	if( NULL == (c = malloc(sizeof(msc_user))))
		goto clean1;

	c->id = strtol(s, &e, 10 );
	if( s == e )
		goto clean2;

	s = e+1;
	if( NULL == (c->name = _msc_fielddup( s, &e )))
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

void msc_user_free( msc_user *c )
{
	if( ! c )
		return;

	free( c->name);
	free(c);
}



msc_it_user *msc_cmd_userlist( mservclient *c )
{
	return _msc_iterate( c, (_msc_converter)_msc_user_parse, 
			"userlist" );
}

msc_user *msc_cmd_userget( mservclient *c, int uid )
{
	return _msc_cmd_conv(c, (_msc_converter)_msc_user_parse, 
			"userget %d", uid);
}

int msc_cmd_usergetname( mservclient *c, const char *name )
{
	return _msc_cmd_int(c, "user2id %s", name);
}

int msc_cmd_usersetpass( mservclient *c, int uid, const char *pass )
{
	return _msc_cmd_succ(c, "usersetpass %d %s", uid, pass);
}

int msc_cmd_usersetright( mservclient *c, int uid, int right )
{
	return _msc_cmd_succ(c, "usersetright %d %d", uid, right);
}

int msc_cmd_userdel( mservclient *c, int uid )
{
	return _msc_cmd_succ(c, "userdel %d", uid);
}

int msc_cmd_useradd( mservclient *c, const char *name )
{
	return _msc_cmd_int(c, "useradd %s", name);
}





