int nfs_permission(struct user_namespace *mnt_userns,
		   struct inode *inode,
		   int mask)
{
	const struct cred *cred = current_cred();
	int res = 0;

	nfs_inc_stats(inode, NFSIOS_VFSACCESS);

	if ((mask & (MAY_READ | MAY_WRITE | MAY_EXEC)) == 0)
		goto out;
	/* Is this sys_access() ? */
	if (mask & (MAY_ACCESS | MAY_CHDIR))
		goto force_lookup;

	switch (inode->i_mode & S_IFMT) {
		case S_IFLNK:
			goto out;
		case S_IFREG:
			if ((mask & MAY_OPEN) &&
			   nfs_server_capable(inode, NFS_CAP_ATOMIC_OPEN))
				return 0;
			break;
		case S_IFDIR:
			/*
			 * Optimize away all write operations, since the server
			 * will check permissions when we perform the op.
			 */
			if ((mask & MAY_WRITE) && !(mask & MAY_READ))
				goto out;
	}

force_lookup:
	if (!NFS_PROTO(inode)->access)
		goto out_notsup;

	res = nfs_do_access(inode, cred, mask);
out:
	if (!res && (mask & MAY_EXEC))
		res = nfs_execute_ok(inode, mask);

	dfprintk(VFS, "NFS: permission(%s/%lu), mask=0x%x, res=%d\n",
		inode->i_sb->s_id, inode->i_ino, mask, res);
	return res;
out_notsup:
	if (mask & MAY_NOT_BLOCK)
		return -ECHILD;

	res = nfs_revalidate_inode(inode, NFS_INO_INVALID_MODE |
						  NFS_INO_INVALID_OTHER);
	if (res == 0)
		res = generic_permission(&init_user_ns, inode, mask);
	goto out;
}