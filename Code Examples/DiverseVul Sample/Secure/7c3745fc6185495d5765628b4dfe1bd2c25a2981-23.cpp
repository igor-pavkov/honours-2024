int validate_headref(const char *path)
{
	struct stat st;
	char buffer[256];
	const char *refname;
	struct object_id oid;
	int fd;
	ssize_t len;

	if (lstat(path, &st) < 0)
		return -1;

	/* Make sure it is a "refs/.." symlink */
	if (S_ISLNK(st.st_mode)) {
		len = readlink(path, buffer, sizeof(buffer)-1);
		if (len >= 5 && !memcmp("refs/", buffer, 5))
			return 0;
		return -1;
	}

	/*
	 * Anything else, just open it and try to see if it is a symbolic ref.
	 */
	fd = open(path, O_RDONLY);
	if (fd < 0)
		return -1;
	len = read_in_full(fd, buffer, sizeof(buffer)-1);
	close(fd);

	if (len < 0)
		return -1;
	buffer[len] = '\0';

	/*
	 * Is it a symbolic ref?
	 */
	if (skip_prefix(buffer, "ref:", &refname)) {
		while (isspace(*refname))
			refname++;
		if (starts_with(refname, "refs/"))
			return 0;
	}

	/*
	 * Is this a detached HEAD?
	 */
	if (!get_oid_hex(buffer, &oid))
		return 0;

	return -1;
}