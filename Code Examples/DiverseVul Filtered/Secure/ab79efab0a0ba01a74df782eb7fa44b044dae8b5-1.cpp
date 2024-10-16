int ovl_copy_up_one(struct dentry *parent, struct dentry *dentry,
		    struct path *lowerpath, struct kstat *stat,
		    struct iattr *attr)
{
	struct dentry *workdir = ovl_workdir(dentry);
	int err;
	struct kstat pstat;
	struct path parentpath;
	struct dentry *upperdir;
	struct dentry *upperdentry;
	const struct cred *old_cred;
	struct cred *override_cred;
	char *link = NULL;

	if (WARN_ON(!workdir))
		return -EROFS;

	ovl_path_upper(parent, &parentpath);
	upperdir = parentpath.dentry;

	err = vfs_getattr(&parentpath, &pstat);
	if (err)
		return err;

	if (S_ISLNK(stat->mode)) {
		link = ovl_read_symlink(lowerpath->dentry);
		if (IS_ERR(link))
			return PTR_ERR(link);
	}

	err = -ENOMEM;
	override_cred = prepare_creds();
	if (!override_cred)
		goto out_free_link;

	override_cred->fsuid = stat->uid;
	override_cred->fsgid = stat->gid;
	/*
	 * CAP_SYS_ADMIN for copying up extended attributes
	 * CAP_DAC_OVERRIDE for create
	 * CAP_FOWNER for chmod, timestamp update
	 * CAP_FSETID for chmod
	 * CAP_CHOWN for chown
	 * CAP_MKNOD for mknod
	 */
	cap_raise(override_cred->cap_effective, CAP_SYS_ADMIN);
	cap_raise(override_cred->cap_effective, CAP_DAC_OVERRIDE);
	cap_raise(override_cred->cap_effective, CAP_FOWNER);
	cap_raise(override_cred->cap_effective, CAP_FSETID);
	cap_raise(override_cred->cap_effective, CAP_CHOWN);
	cap_raise(override_cred->cap_effective, CAP_MKNOD);
	old_cred = override_creds(override_cred);

	err = -EIO;
	if (lock_rename(workdir, upperdir) != NULL) {
		pr_err("overlayfs: failed to lock workdir+upperdir\n");
		goto out_unlock;
	}
	upperdentry = ovl_dentry_upper(dentry);
	if (upperdentry) {
		unlock_rename(workdir, upperdir);
		err = 0;
		/* Raced with another copy-up?  Do the setattr here */
		if (attr) {
			mutex_lock(&upperdentry->d_inode->i_mutex);
			err = notify_change(upperdentry, attr, NULL);
			mutex_unlock(&upperdentry->d_inode->i_mutex);
		}
		goto out_put_cred;
	}

	err = ovl_copy_up_locked(workdir, upperdir, dentry, lowerpath,
				 stat, attr, link);
	if (!err) {
		/* Restore timestamps on parent (best effort) */
		ovl_set_timestamps(upperdir, &pstat);
	}
out_unlock:
	unlock_rename(workdir, upperdir);
out_put_cred:
	revert_creds(old_cred);
	put_cred(override_cred);

out_free_link:
	if (link)
		free_page((unsigned long) link);

	return err;
}