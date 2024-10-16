static char *sqfs_dirname(char *path)
{
	char *fname;

	fname = sqfs_basename(path);
	--fname;
	*fname = '\0';

	return path;
}