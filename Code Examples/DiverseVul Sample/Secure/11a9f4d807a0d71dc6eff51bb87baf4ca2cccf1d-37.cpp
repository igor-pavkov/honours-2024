static int is_dir_file(const char *buf, const char *dir, const char *file)
{
	int len = strlen(dir);
	if (strncmp(buf, dir, len) || !is_dir_sep(buf[len]))
		return 0;
	while (is_dir_sep(buf[len]))
		len++;
	return !strcmp(buf + len, file);
}