static char *get_username(void)
{
	struct passwd pwent;
	struct passwd *pwentp = NULL;
	char *buf;
	char *username;
	size_t bufsize;
	int ret;

	bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
	if (bufsize == -1)
		bufsize = 1024;

	buf = malloc(bufsize);
	if (!buf)
		return NULL;

	ret = getpwuid_r(getuid(), &pwent, buf, bufsize, &pwentp);
	if (!pwentp) {
		if (ret == 0)
			usernic_error("%s", "Could not find matched password record\n");

		usernic_error("Failed to get username: %s(%u)\n", strerror(errno), getuid());
		free(buf);
		return NULL;
	}

	username = strdup(pwent.pw_name);
	free(buf);

	return username;
}