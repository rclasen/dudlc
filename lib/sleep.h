#ifndef _MSERVCLIENT_SLEEP_H
#define _MSERVCLIENT_SLEEP_H

#include <mservclient/client.h>

int msc_cmd_sleep( mservclient *c );
int msc_cmd_sleepset( mservclient *c, int delay );

#endif
