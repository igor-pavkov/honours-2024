int nfs_unlink(struct inode *dir, struct dentry *dentry)
{
	int error;
	int need_rehash = 0;

	dfprintk(VFS, "NFS: unlink(%s/%lu, %pd)\n", dir->i_sb->s_id,
		dir->i_ino, dentry);

	trace_nfs_unlink_enter(dir, dentry);
	spin_lock(&dentry->d_lock);
	if (d_count(dentry) > 1) {
		spin_unlock(&dentry->d_lock);
		/* Start asynchronous writeout of the inode */
		write_inode_now(d_inode(dentry), 0);
		error = nfs_sillyrename(dir, dentry);
		goto out;
	}
	if (!d_unhashed(dentry)) {
		__d_drop(dentry);
		need_rehash = 1;
	}
	spin_unlock(&dentry->d_lock);
	error = nfs_safe_remove(dentry);
	nfs_dentry_remove_handle_error(dir, dentry, error);
	if (need_rehash)
		d_rehash(dentry);
out:
	trace_nfs_unlink_exit(dir, dentry, error);
	return error;
}