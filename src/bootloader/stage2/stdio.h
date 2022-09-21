#pragma once

void putc(char c);
void puts(const char* str);
void _cdecl printf(const char* fmt, ...);
int* print_number(int* argp, int length, bool sign, int radix);