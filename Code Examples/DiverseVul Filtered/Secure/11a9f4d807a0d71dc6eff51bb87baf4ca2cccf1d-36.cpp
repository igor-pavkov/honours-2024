static int get_st_mode_bits(const char *path, int *mode)
{
	struct stat st;
	if (lstat(path, &st) < 0)
		return -1;
	*mode = st.st_mode;
	return 0;
}