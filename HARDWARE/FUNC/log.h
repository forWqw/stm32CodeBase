#ifndef __LOG_H
#define __LOG_H

typedef enum log_leavel
{
	DEBUG_INIT = 0,
	DEBUG_ERROR
} log_leavel_e;

void log_printf(int debug_level, char *fmt, ...);

#endif
