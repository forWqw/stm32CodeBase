#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "log.h"

int g_debug_level = DEBUG_INIT;
char buf[128] = {0};

char *itoa(int i, char *str)
{
    int mod, div = fabs(i), index = 0;
    char *start, *end, temp;

    do
    {
        mod = div % 10;
        str[index++] = '0' + mod;
        div = div / 10;
    }while(div != 0);

    if (i < 0)
        str[index++] = '-';

    str[index] = '\0';

    for (start = str, end = str + strlen(str) - 1;
        start < end; start++, end--)
    {
        temp    = *start;
        *start    = *end;
        *end    = temp;
    }
    
    return str;
}

void log_printf(int debug_level, char *fmt, ...)
{
	char str[100];
    unsigned int len, i, index;
    int iTemp;
    char *strTemp;
    va_list args;

    va_start(args, fmt);
    len = strlen(fmt);
    for (i=0, index=0; i<len; i++)
    {
        if (fmt[i] != '%')    /* ·Ç¸ñÊ½»¯²ÎÊý */
        {
            str[index++] = fmt[i];
        }
        else                /* ¸ñÊ½»¯²ÎÊý */
        {
            switch(fmt[i+1])
            {
            case 'd':        /* ÕûÐÍ */
            case 'D':
                iTemp = va_arg(args, int);
                strTemp = itoa(iTemp, str+index);
                index += strlen(strTemp);
                i++;
                break;
            case 's':        /* ×Ö·û´® */
            case 'S':
                strTemp = va_arg(args, char*);
                strcpy(str + index, strTemp);
                index += strlen(strTemp);
                i++;
                break;
            default:
                str[index++] = fmt[i];
            }
        }
    }
    str[index] = '\0';
    va_end(args);

    printf(str);
}
