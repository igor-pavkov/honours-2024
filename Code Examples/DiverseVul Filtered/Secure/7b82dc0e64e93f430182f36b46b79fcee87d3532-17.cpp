struct file *nameidata_to_filp(struct nameidata *nd, int flags)
{
	struct file *filp;

	/* Pick up the filp from the open intent */
	filp = nd->intent.open.file;
	/* Has the filesystem initialised the file for us? */
	if (filp->f_path.dentry == NULL)
		filp = __dentry_open(nd->dentry, nd->mnt, flags, filp, NULL);
	else
		path_release(nd);
	return filp;
}