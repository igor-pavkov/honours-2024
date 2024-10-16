int mingw_skip_dos_drive_prefix(char **path)
{
	int ret = has_dos_drive_prefix(*path);
	*path += ret;
	return ret;
}