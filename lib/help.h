#ifndef _MSERVCLIENT_HELP_H
#define _MSERVCLIENT_HELP_H

#include <mservclient/client.h>
#include <mservclient/iterate.h>


#define msc_it_help	_msc_iter
#define msc_it_help_cur(x)	(char*)_msc_it_cur((_msc_iter*)x)
#define msc_it_help_next(x)	(char*)_msc_it_next((_msc_iter*)x)
#define msc_it_help_done(x)	_msc_it_done((_msc_iter*)x)

msc_it_help *msc_cmd_help( mservclient *con );

#endif

