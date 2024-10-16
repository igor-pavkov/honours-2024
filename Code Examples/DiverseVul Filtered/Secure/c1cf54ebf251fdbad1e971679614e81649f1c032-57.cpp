static bool create_db_dir(char *fnam)
{
	int ret;
	char *p;
	size_t len;

	len = strlen(fnam);
	p = alloca(len + 1);
	(void)strlcpy(p, fnam, len + 1);
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