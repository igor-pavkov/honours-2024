int nfs_rmdir(struct inode *dir, struct dentry *dentry)
{
	int error;

	dfprintk(VFS, "NFS: rmdir(%s/%lu), %pd\n",
			dir->i_sb->s_id, dir->i_ino, dentry);

	trace_nfs_rmdir_enter(dir, dentry);
	if (d_really_is_positive(dentry)) {
		down_write(&NFS_I(d_inode(dentry))->rmdir_sem);
		error = NFS_PROTO(dir)->rmdir(dir, &dentry->d_name);
		/* Ensure the VFS deletes this inode */
		switch (error) {
		case 0:
			clear_nlink(d_inode(dentry));
			break;
		case -ENOENT:
			nfs_dentry_handle_enoent(dentry);
		}
		up_write(&NFS_I(d_inode(dentry))->rmdir_sem);
	} else
		error = NFS_PROTO(dir)->rmdir(dir, &dentry->d_name);
	nfs_dentry_remove_handle_error(dir, dentry, error);
	trace_nfs_rmdir_exit(dir, dentry, error);

	return error;
}