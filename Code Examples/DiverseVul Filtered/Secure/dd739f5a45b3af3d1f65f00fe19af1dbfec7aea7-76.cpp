static char* findSeparator(char *p) {
	char *q = strchr (p, '+');
	if (q) {
		return q;
	}
	return strchr (p, '-');
}