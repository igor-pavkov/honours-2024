static pid_t mingw_spawnv(const char *cmd, const char **argv, int prepend_cmd)
{
	return mingw_spawnve_fd(cmd, argv, NULL, NULL, prepend_cmd, 0, 1, 2);
}