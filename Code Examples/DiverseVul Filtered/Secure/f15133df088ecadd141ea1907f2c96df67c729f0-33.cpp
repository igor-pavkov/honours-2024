struct dentry *kern_path_create(int dfd, const char *pathname,
				struct path *path, unsigned int lookup_flags)
{
	struct filename *filename = getname_kernel(pathname);
	struct dentry *res;

	if (IS_ERR(filename))
		return ERR_CAST(filename);
	res = filename_create(dfd, filename, path, lookup_flags);
	putname(filename);
	return res;
}