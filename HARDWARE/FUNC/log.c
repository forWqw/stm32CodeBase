#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "log.h"

int g_debug_level = DEBUG_INIT;
char buf[128] = {0};

int log_printf(int debug_level, char *fmt, ...)
{
	va_list args;
	int ret = -1;
	FILE *fp = NULL;

	if(debug_level > g_debug_level)
	{
		return 0;
	}
	
	va_start(args, fmt);
	ret = vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);
	return ret;
}
