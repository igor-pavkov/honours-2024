int is_dir(const char *path)
{
	struct stat statbuf;
	int ret;

	ret = stat(path, &statbuf);
	if (ret == 0 && S_ISDIR(statbuf.st_mode))
		return 1;

	return 0;
}