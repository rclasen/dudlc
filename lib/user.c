#include <stdlib.h>

#include <mservclient/proto.h>
#include <mservclient/event.h>
#include <mservclient/user.h>

static msc_client *_msc_client_parse( const char *line, char **end )
{
	msc_client *c;

	const char *s;
	char *e;

	(const char*)e = s = _msc_skipspace(line);

	if( NULL == (c = malloc(sizeof(msc_client))))
		goto clean1;

	c->id = strtol(s, &e, 10 );
	if( s == e )
		goto clean2;

	s = _msc_skipspace(e);
	c->uid = strtol(s, &e, 10 );
	if( s == e )
		goto clean2;

	s = _msc_skipspace(e);
	if( NULL == (c->addr = _msc_fielddup( s, &e )))
		goto clean2;
	
	return c;
clean2:
	free(c);
clean1:
	if( end ) (const char *)*end = line;
	return NULL;
}

void msc_client_free( msc_client *c )
{
	if( ! c )
		return;

	free( c->addr);
	free(c);
}

msc_it_client *msc_cmd_who( mservclient *c )
{
	return _msc_iterate( c, (_msc_converter)_msc_client_parse, "who" );
}

int msc_cmd_kick( mservclient *c, int uid )
{
	if( _msc_cmd(c, "kick %d", uid ))
		return -1;
	if( *_msc_rcode(c) != '2' )
		return -1;
	return 0;
}

void _msc_bcast_user( mservclient *c, const char *line )
{
	msc_client *u;

	switch(line[2]){
		case '0': /* login */
			if( NULL != (u = _msc_client_parse( line+4, NULL ))){
				_MSC_EVENT(c,login,c,u);
				msc_client_free(u);
			}
			break;

		case '1': /* logout */
			if( NULL != (u = _msc_client_parse( line+4, NULL ))){
				_MSC_EVENT(c,logout,c,u);
				msc_client_free(u);
			}
			break;


		default:
			_MSC_EVENT(c,bcast,c,line);
	}
	(void)c;
}
