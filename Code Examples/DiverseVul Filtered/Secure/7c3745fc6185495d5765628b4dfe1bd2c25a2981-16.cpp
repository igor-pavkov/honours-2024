static const char *cleanup_path(const char *path)
{
	/* Clean it up */
	if (skip_prefix(path, "./", &path)) {
		while (*path == '/')
			path++;
	}
	return path;
}