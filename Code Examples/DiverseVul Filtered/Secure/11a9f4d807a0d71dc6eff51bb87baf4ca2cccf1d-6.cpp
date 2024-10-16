static int dir_prefix(const char *buf, const char *dir)
{
	int len = strlen(dir);
	return !strncmp(buf, dir, len) &&
		(is_dir_sep(buf[len]) || buf[len] == '\0');
}