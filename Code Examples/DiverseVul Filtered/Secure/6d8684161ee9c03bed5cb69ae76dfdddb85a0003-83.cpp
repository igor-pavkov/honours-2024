static char *lookup_prog(const char *dir, int dirlen, const char *cmd,
			 int isexe, int exe_only)
{
	char path[MAX_PATH];
	snprintf(path, sizeof(path), "%.*s\\%s.exe", dirlen, dir, cmd);

	if (!isexe && access(path, F_OK) == 0)
		return xstrdup(path);
	path[strlen(path)-4] = '\0';
	if ((!exe_only || isexe) && access(path, F_OK) == 0)
		if (!(GetFileAttributes(path) & FILE_ATTRIBUTE_DIRECTORY))
			return xstrdup(path);
	return NULL;
}