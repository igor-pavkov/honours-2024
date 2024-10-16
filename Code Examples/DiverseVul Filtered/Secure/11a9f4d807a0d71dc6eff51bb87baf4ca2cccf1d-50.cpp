static int check_common(const char *unmatched, void *value, void *baton)
{
	struct common_dir *dir = value;

	if (!dir)
		return 0;

	if (dir->is_dir && (unmatched[0] == 0 || unmatched[0] == '/'))
		return !dir->exclude;

	if (!dir->is_dir && unmatched[0] == 0)
		return !dir->exclude;

	return 0;
}