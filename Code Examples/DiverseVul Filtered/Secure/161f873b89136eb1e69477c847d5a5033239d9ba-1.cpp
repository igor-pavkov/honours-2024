static int do_handle_to_path(int mountdirfd, struct file_handle *handle,
			     struct path *path)
{
	int retval = 0;
	int handle_dwords;

	path->mnt = get_vfsmount_from_fd(mountdirfd);
	if (IS_ERR(path->mnt)) {
		retval = PTR_ERR(path->mnt);
		goto out_err;
	}
	/* change the handle size to multiple of sizeof(u32) */
	handle_dwords = handle->handle_bytes >> 2;
	path->dentry = exportfs_decode_fh(path->mnt,
					  (struct fid *)handle->f_handle,
					  handle_dwords, handle->handle_type,
					  vfs_dentry_acceptable, NULL);
	if (IS_ERR(path->dentry)) {
		retval = PTR_ERR(path->dentry);
		goto out_mnt;
	}
	return 0;
out_mnt:
	mntput(path->mnt);
out_err:
	return retval;
}