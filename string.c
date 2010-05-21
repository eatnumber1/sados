#include "string.h"
#include "headers.h"

int strlen( const char *str ) {
	int len = 0;
	for( char c = *str; c != '\0'; c = *(++str) ) len++;
	return len;
}

int strcmp( const char *str1, const char *str2 ) {
	int i;
	for( i = 0; str1[i] == str2[i]; i++ ) if( str1[i] == '\0' ) return 0;
	return str1[i] - str2[i];
}

char *strtok( char *str, const char delim ) {
	static char *saved_str;
	if( str != NULL ) saved_str = str;
	if( *saved_str == NULL ) return NULL;
	char *original_str = saved_str;
	for( char c = *saved_str; c != delim && c != NULL; c = *++saved_str );
	*saved_str++ = NULL;
	return original_str;
}

int atoi( char *str ) {
	int pow = 1;
	int num = 0;
	char *ptr = str;
	for( char c = *ptr;; c = *++ptr ) {
		if( c == NULL ) break;
		num += (c - 48) * pow;
		pow *= 10;
	}
	return num;
}

void itoa( char *buf, int count, int num ) {
	int c = 0, power = 1;
	if( num == 0 ) {
		buf[0] = '0';
		buf[1] = '\0';
		return;
	}
	while( num / power != 0 ) power *= 10;
	while( c != count - 1 && power != 1 ) {
		power /= 10;
		buf[c++] = ((num / power) % 10) + 48;
	}
	buf[c] = '\0';
}
