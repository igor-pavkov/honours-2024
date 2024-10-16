static char *path_lookup(const char *cmd, int exe_only)
{
	const char *path;
	char *prog = NULL;
	int len = strlen(cmd);
	int isexe = len >= 4 && !strcasecmp(cmd+len-4, ".exe");

	if (strchr(cmd, '/') || strchr(cmd, '\\'))
		return xstrdup(cmd);

	path = mingw_getenv("PATH");
	if (!path)
		return NULL;

	while (!prog) {
		const char *sep = strchrnul(path, ';');
		int dirlen = sep - path;
		if (dirlen)
			prog = lookup_prog(path, dirlen, cmd, isexe, exe_only);
		if (!*sep)
			break;
		path = sep + 1;
	}

	return prog;
}