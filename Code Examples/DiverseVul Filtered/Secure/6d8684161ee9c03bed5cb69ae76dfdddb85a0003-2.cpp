int mingw_execvp(const char *cmd, char *const *argv)
{
	char *prog = path_lookup(cmd, 0);

	if (prog) {
		mingw_execv(prog, argv);
		free(prog);
	} else
		errno = ENOENT;

	return -1;
}