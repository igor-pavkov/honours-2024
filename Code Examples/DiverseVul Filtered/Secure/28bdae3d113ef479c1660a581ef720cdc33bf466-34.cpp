static int get_string_opt(const char *s, unsigned len, const char *opt,
			  char **val)
{
	int i;
	unsigned opt_len = strlen(opt);
	char *d;

	if (*val)
		free(*val);
	*val = (char *) malloc(len - opt_len + 1);
	if (!*val) {
		fprintf(stderr, "%s: failed to allocate memory\n", progname);
		return 0;
	}

	d = *val;
	s += opt_len;
	len -= opt_len;
	for (i = 0; i < len; i++) {
		if (s[i] == '\\' && i + 1 < len)
			i++;
		*d++ = s[i];
	}
	*d = '\0';
	return 1;
}