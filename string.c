int strlen( const char *str ) {
	int len = 0;
	for( char c = *str; c != '\0'; c = *(++str) ) len++;
	return len;
}
