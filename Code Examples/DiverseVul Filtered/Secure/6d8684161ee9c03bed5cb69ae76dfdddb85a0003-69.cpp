int mingw_stat(const char *file_name, struct stat *buf)
{
	return do_stat_internal(1, file_name, buf);
}