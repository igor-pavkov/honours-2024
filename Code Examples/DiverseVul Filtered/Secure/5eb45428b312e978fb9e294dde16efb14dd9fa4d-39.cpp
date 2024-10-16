static bool create_db_dir(char *fnam)
{
	int ret;
	char *p;

	p = alloca(strlen(fnam) + 1);
	strcpy(p, fnam);
	fnam = p;
	p = p + 1;

again:
	while (*p && *p != '/')
		p++;
	if (!*p)
		return true;

	*p = '\0';

	ret = mkdir(fnam, 0755);
	if (ret < 0 && errno != EEXIST) {
		usernic_error("Failed to create %s: %s\n", fnam,
			      strerror(errno));
		*p = '/';
		return false;
	}
	*(p++) = '/';

	goto again;
}