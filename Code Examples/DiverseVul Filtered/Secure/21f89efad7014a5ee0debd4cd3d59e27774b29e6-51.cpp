static char *check_str(char *line, size_t line_size, const char *needle, size_t needle_size)
{
	char *p;
	unsigned n;

	while (c_isspace(*line)) {
		line++;
		line_size--;
	}

	if (line[0] == '#' || needle_size >= line_size)
		return NULL;

	if (memcmp(line, needle, needle_size) == 0) {
		p = &line[needle_size];
		while (c_isspace(*p)) {
			p++;
		}
		if (*p != '=') {
			return NULL;
		} else
			p++;

		while (c_isspace(*p)) {
			p++;
		}

		n = strlen(p);

		if (n > 1 && p[n-1] == '\n') {
			n--;
			p[n] = 0;
		}

		if (n > 1 && p[n-1] == '\r') {
			n--;
			p[n] = 0;
		}
		return p;
	}

	return NULL;
}