static inline char *eon(char *n) {
	while (*n && (*n >= '0' && *n <= '9')) {
		n++;
	}
	return n;
}