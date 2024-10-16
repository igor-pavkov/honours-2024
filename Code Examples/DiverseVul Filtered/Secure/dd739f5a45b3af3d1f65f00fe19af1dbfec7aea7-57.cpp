static char *find_eoq(char *p) {
	for (; *p; p++) {
		if (*p == '"') {
			break;
		}
		if (*p == '\\' && p[1] == '"') {
			p++;
		}
	}
	return p;
}