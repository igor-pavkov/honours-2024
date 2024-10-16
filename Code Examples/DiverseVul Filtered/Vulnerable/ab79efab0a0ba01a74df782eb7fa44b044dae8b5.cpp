static int ovl_copy_up_locked(struct dentry *workdir, struct dentry *upperdir,
			      struct dentry *dentry, struct path *lowerpath,
			      struct kstat *stat, struct iattr *attr,
			      const char *link)
{
	struct inode *wdir = workdir->d_inode;
	struct inode *udir = upperdir->d_inode;
	struct dentry *newdentry = NULL;
	struct dentry *upper = NULL;
	umode_t mode = stat->mode;
	int err;

	newdentry = ovl_lookup_temp(workdir, dentry);
	err = PTR_ERR(newdentry);
	if (IS_ERR(newdentry))
		goto out;

	upper = lookup_one_len(dentry->d_name.name, upperdir,
			       dentry->d_name.len);
	err = PTR_ERR(upper);
	if (IS_ERR(upper))
		goto out1;

	/* Can't properly set mode on creation because of the umask */
	stat->mode &= S_IFMT;
	err = ovl_create_real(wdir, newdentry, stat, link, NULL, true);
	stat->mode = mode;
	if (err)
		goto out2;

	if (S_ISREG(stat->mode)) {
		struct path upperpath;
		ovl_path_upper(dentry, &upperpath);
		BUG_ON(upperpath.dentry != NULL);
		upperpath.dentry = newdentry;

		err = ovl_copy_up_data(lowerpath, &upperpath, stat->size);
		if (err)
			goto out_cleanup;
	}

	err = ovl_copy_xattr(lowerpath->dentry, newdentry);
	if (err)
		goto out_cleanup;

	mutex_lock(&newdentry->d_inode->i_mutex);
	err = ovl_set_attr(newdentry, stat);
	if (!err && attr)
		err = notify_change(newdentry, attr, NULL);
	mutex_unlock(&newdentry->d_inode->i_mutex);
	if (err)
		goto out_cleanup;

	err = ovl_do_rename(wdir, newdentry, udir, upper, 0);
	if (err)
		goto out_cleanup;

	ovl_dentry_update(dentry, newdentry);
	newdentry = NULL;

	/*
	 * Non-directores become opaque when copied up.
	 */
	if (!S_ISDIR(stat->mode))
		ovl_dentry_set_opaque(dentry, true);
out2:
	dput(upper);
out1:
	dput(newdentry);
out:
	return err;

out_cleanup:
	ovl_cleanup(wdir, newdentry);
	goto out;
}