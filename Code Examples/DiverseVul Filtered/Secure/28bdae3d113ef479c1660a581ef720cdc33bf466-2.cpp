static int add_option(char **optsp, const char *opt, unsigned expand)
{
	char *newopts;
	if (*optsp == NULL)
		newopts = strdup(opt);
	else {
		unsigned oldsize = strlen(*optsp);
		unsigned newsize = oldsize + 1 + strlen(opt) + expand + 1;
		newopts = (char *) realloc(*optsp, newsize);
		if (newopts)
			sprintf(newopts + oldsize, ",%s", opt);
	}
	if (newopts == NULL) {
		fprintf(stderr, "%s: failed to allocate memory\n", progname);
		return -1;
	}
	*optsp = newopts;
	return 0;
}