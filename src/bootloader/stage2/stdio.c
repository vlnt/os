#include "stdio.h"
#include "x86.h"

void putc(char c)
{
    x86_Video_WriteCharTeletype(c, 0);
}

void puts(const char* str)
{
    while(*str)
    {
        putc(*str);
        str++;
    }
}

#define PRINTF_STATE_NORMAL       0
#define PRINTF_STATE_LENGTH       1
#define PRINTF_STATE_LENGTH_SHORT 2
#define PRINTF_STATE_LENGTH_LONG  3
#define PRINTF_STATE_SPEC         4

#define PRINTF_LENGTH_DEFAULT     0
#define PRINTF_LENGTH_SHORT_SHORT 1
#define PRINTF_LENGTH_SHORT       2
#define PRINTF_LENGTH_LONG        3
#define PRINTF_LENGTH_LONG_LONG   4

void _cdecl printf(const char* fmt, ...)
{
    int* argp = (int*)&fmt;
    int state = PRINTF_STATE_NORMAL;
    int length = PRINTF_LENGTH_DEFAULT;

    while(*fmt)
    {
        switch(state)
        {
            case PRINTF_STATE_NORMAL:
                switch(*fmt)
                {
                    case '%': state = PRINTF_STATE_LENGTH;
                        break;
                    default : putc(*fmt); 
                        break; 
                }
                break;
            case PRINTF_STATE_LENGTH:
                switch(*fmt)
                {
                    case 'h': length = PRINTF_LENGTH_SHORT;
                              state = PRINTF_STATE_LENGTH_SHORT;
                        break;
                    case 'l': length = PRINTF_LENGTH_LONG;
                              state = PRINTF_STATE_LENGTH_LONG;
                        break;
                    default : goto PRINTF_STATE_SPEC_;
                        break; 
                }
                break;
            case PRINTF_STATE_LENGTH_SHORT:
                if(*fmt == 'h')
                {
                    length = PRINTF_LENGTH_SHORT_SHORT;
                    state = PRINTF_STATE_SPEC;
                }
                else goto PRINTF_STATE_SPEC_;
                break;
            case PRINTF_STATE_LENGTH_LONG:
                if(*fmt == 'h')
                {
                    length = PRINTF_LENGTH_LONG_LONG;
                    state = PRINTF_STATE_SPEC;
                }
                else goto PRINTF_STATE_SPEC_;
                break;
        }

        fmt++;
    }
}