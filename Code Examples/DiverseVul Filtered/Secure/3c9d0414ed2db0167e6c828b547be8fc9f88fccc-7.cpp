static const char *set_work_tree(const char *dir)
{
	char buffer[PATH_MAX + 1];

	if (!getcwd(buffer, sizeof(buffer)))
		die ("Could not get the current working directory");
	git_work_tree_cfg = xstrdup(buffer);
	inside_work_tree = 1;

	return NULL;
}