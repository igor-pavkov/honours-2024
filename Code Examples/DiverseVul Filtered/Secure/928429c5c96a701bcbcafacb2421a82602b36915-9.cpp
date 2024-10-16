static int tree_error(const char *str, const char *path)
{
	if (path)
		giterr_set(GITERR_TREE, "%s - %s", str, path);
	else
		giterr_set(GITERR_TREE, "%s", str);
	return -1;
}