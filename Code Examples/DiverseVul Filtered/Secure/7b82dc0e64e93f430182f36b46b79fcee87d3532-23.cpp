static struct file *do_filp_open(int dfd, const char *filename, int flags,
				 int mode)
{
	int namei_flags, error;
	struct nameidata nd;

	namei_flags = flags;
	if ((namei_flags+1) & O_ACCMODE)
		namei_flags++;

	error = open_namei(dfd, filename, namei_flags, mode, &nd);
	if (!error)
		return nameidata_to_filp(&nd, flags);

	return ERR_PTR(error);
}