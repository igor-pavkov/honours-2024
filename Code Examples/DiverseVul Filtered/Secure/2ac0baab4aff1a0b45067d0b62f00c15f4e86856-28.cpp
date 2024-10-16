static int sqfs_clean_base_path(char **base, int count, int updir)
{
	int i;

	for (i = count - updir - 1; i < count; i++)
		free(base[i]);

	return count - updir - 1;
}