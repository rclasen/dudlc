#include <stdlib.h>
#include <string.h>

#include <mservclient/proto.h>
#include <mservclient/event.h>
#include <mservclient/tag.h>

static msc_tag *_msc_tag_parse( const char *line, char **end )
{
	msc_tag *n;
	const char *s;
	char *e;

	/* why isn't strtol's endptr defined as (const char*) ?
	 * now we have to cast the const hackishly away */
	(const char*)e = s = line;

	if( NULL == (n = malloc(sizeof(msc_track)))){
		goto clean1;
	}

	n->id = strtol( s, &e, 10 );
	if( s == e )
		goto clean2;

	s = e+1;
	if( NULL == ( n->name = _msc_fielddup(s, &e)))
		goto clean2;

	s = e+1;
	if( NULL == ( n->desc = _msc_fielddup(s, &e)))
		goto clean3;


	if( end ) *end = e;
	return n;

clean3:
	free(n->name);
clean2:
	free(n);
clean1:
	if( end ) (const char *)*end = line;
	return NULL;
}

void msc_tag_free( msc_tag *n )
{
	if( ! n )
		return;
	free(n->name);
	free(n->desc);
	free(n);
}

msc_tag *msc_cmd_tagget( mservclient *c, int id )
{
	return _msc_cmd_conv(c, (_msc_converter)_msc_tag_parse, 
			"tagget %d", id );
}

msc_tag *msc_cmd_tagname( mservclient *c, const char *name )
{
	return _msc_cmd_conv(c, (_msc_converter)_msc_tag_parse, 
			"tagname %s", name );
}


msc_it_tag *msc_cmd_taglist( mservclient *c )
{
	return _msc_iterate(c, (_msc_converter)_msc_tag_parse, "taglist" );
}

int msc_cmd_tagadd( mservclient *c, const char *name )
{
	return _msc_cmd_int(c, "tagadd %s", name );
}

int msc_cmd_tagsetname( mservclient *c, int id, const char *name )
{
	return _msc_cmd_succ( c, "tagsetname %d %s", id, name );
}

int msc_cmd_tagsetdesc( mservclient *c, int id, const char *desc )
{
	return _msc_cmd_succ( c, "tagsetdesc %d %s", id, desc );
}

int msc_cmd_tagdel( mservclient *c, int id )
{
	return _msc_cmd_succ( c, "tagdel %d", id );
}


msc_it_tag *msc_cmd_tracktags( mservclient *c, int tid )
{
	return _msc_iterate(c, (_msc_converter)_msc_tag_parse, 
			"tracktags %d", tid );
}

int msc_cmd_tracktagged( mservclient *c, int tid, int id )
{
	return _msc_cmd_int(c, "tracktagged %d %d", tid, id );
}

int msc_cmd_tracktagset( mservclient *c, int tid, int id )
{
	return _msc_cmd_succ( c, "tracktagset %d %d", tid, id );
}

int msc_cmd_tracktagdel( mservclient *c, int tid, int id )
{
	return _msc_cmd_succ( c, "tracktagdel %d %d", tid, id );
}

void _msc_bcast_tag( mservclient *c, const char *line )
{
	msc_tag *t;

	switch(line[2]){
		case '0': /* tag changed */
			if( NULL == (t = _msc_tag_parse(line+4,NULL)))
				break;
			_MSC_EVENT(c,tagchange,c,t);
			msc_tag_free(t);
			break;

		case '1': /* tag changed */
			if( NULL == (t = _msc_tag_parse(line+4,NULL)))
				break;
			_MSC_EVENT(c,tagdel,c,t);
			msc_tag_free(t);
			break;


		default:
			_MSC_EVENT(c,bcast,c,line);
	}
}




