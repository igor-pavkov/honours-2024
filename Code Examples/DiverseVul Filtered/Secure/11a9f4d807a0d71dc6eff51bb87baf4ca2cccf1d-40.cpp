static int have_same_root(const char *path1, const char *path2)
{
	int is_abs1, is_abs2;

	is_abs1 = is_absolute_path(path1);
	is_abs2 = is_absolute_path(path2);
	return (is_abs1 && is_abs2 && tolower(path1[0]) == tolower(path2[0])) ||
	       (!is_abs1 && !is_abs2);
}