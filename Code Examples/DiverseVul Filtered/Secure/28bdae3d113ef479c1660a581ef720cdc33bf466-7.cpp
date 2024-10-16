static int chdir_to_parent(char *copy, const char **lastp)
{
	char *tmp;
	const char *parent;
	char buf[65536];
	int res;

	tmp = strrchr(copy, '/');
	if (tmp == NULL || tmp[1] == '\0') {
		fprintf(stderr, "%s: internal error: invalid abs path: <%s>\n",
			progname, copy);
		return -1;
	}
	if (tmp != copy) {
		*tmp = '\0';
		parent = copy;
		*lastp = tmp + 1;
	} else if (tmp[1] != '\0') {
		*lastp = tmp + 1;
		parent = "/";
	} else {
		*lastp = ".";
		parent = "/";
	}

	res = chdir(parent);
	if (res == -1) {
		fprintf(stderr, "%s: failed to chdir to %s: %s\n",
			progname, parent, strerror(errno));
		return -1;
	}

	if (getcwd(buf, sizeof(buf)) == NULL) {
		fprintf(stderr, "%s: failed to obtain current directory: %s\n",
			progname, strerror(errno));
		return -1;
	}
	if (strcmp(buf, parent) != 0) {
		fprintf(stderr, "%s: mountpoint moved (%s -> %s)\n", progname,
			parent, buf);
		return -1;

	}

	return 0;
}