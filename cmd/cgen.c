static char *duc_cgen_raw( dudlc *con, const char *text, int state )
{
	static char **helpv = NULL;
	static int last = 0;

	if( state == 0 ){
		duc_it_help *it;
		char *s;
		unsigned int len;
		int total = 0;
		int used = 0;

		len = strlen(text);

		it = duc_cmd_help(con);
		for( s = duc_it_help_cur(it); s; 
				s = duc_it_help_next(it) ){

			if( 0 != strncasecmp( s, text, len) ){
				free(s);
				continue;
			}

			if( used >= total ){
				char **tmp;

				total += 10;
				tmp = realloc( helpv, sizeof(char*) * 
						(total +1) );
				if( tmp == NULL ){
					free(s);
					break;
				}

				helpv = tmp;
			}

			helpv[used++] = s;
		}
		duc_it_help_done(it);

		if( helpv ){
			helpv[used] = NULL;
			last = 0;
			return *helpv;
		
		}

		last = 0;
		return NULL;
	}

	if( ! helpv )
		return NULL;

	if( ! helpv[++last] ){
		free(helpv);
		helpv = NULL;
		last = 0;
		return NULL;
	}

	return helpv[last];
}

/* toplevel completer */
static char *duc_cgen_top( dudlc *con, const char *text, int state )
{
	static int c;
	static unsigned int len;
	char *name;

	/* TODO use "help" */
	if( state == 0 ){
		c = 0;
		len = strlen(text);
	}

	while( NULL != (name = commands[c].name) ){
		c++;
		
		if( strncasecmp(text,name,len) == 0 )
			return strdup(name);
	}

	return NULL;
	(void) con;
}

t_generator duc_cgen( dudlc *con, const char *line, unsigned int pos )
{
	int len;
	t_command *cmd;

	(void)con;

	/* skip leading whitespace */
	len = strspn( line, WSPACE );
	line += len;
	pos -= len;

	/* begin of line -> complete the command name */
	if( pos == 0 )
		return cgen_top;

	/* else try to complete command arguments */
	// TODO: make completer finding more elegant
	len = strcspn( line, WSPACE );
	if( NULL != (cmd = command_nfind( line, len )))
		return cmd->gen;

	return NULL;
}


