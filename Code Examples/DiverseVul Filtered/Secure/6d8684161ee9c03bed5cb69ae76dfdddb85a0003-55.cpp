int mingw_lstat(const char *file_name, struct stat *buf)
{
	return do_stat_internal(0, file_name, buf);
}