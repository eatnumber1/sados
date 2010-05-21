#ifndef _STRING_H
#define _STRING_H

#include "bool.h"

int strlen( const char *str );
int strcmp( const char *str, const char *str2 );
char *strtok( char *str, const char delim );
int atoi( char *str );
void itoa( char *buf, int count, int num );

#endif
