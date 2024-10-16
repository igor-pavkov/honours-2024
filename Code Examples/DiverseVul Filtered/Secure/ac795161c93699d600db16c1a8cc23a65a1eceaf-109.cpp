int nfs_lookup_verify_inode(struct inode *inode, unsigned int flags)
{
	struct nfs_server *server = NFS_SERVER(inode);
	int ret;

	if (IS_AUTOMOUNT(inode))
		return 0;

	if (flags & LOOKUP_OPEN) {
		switch (inode->i_mode & S_IFMT) {
		case S_IFREG:
			/* A NFSv4 OPEN will revalidate later */
			if (server->caps & NFS_CAP_ATOMIC_OPEN)
				goto out;
			fallthrough;
		case S_IFDIR:
			if (server->flags & NFS_MOUNT_NOCTO)
				break;
			/* NFS close-to-open cache consistency validation */
			goto out_force;
		}
	}

	/* VFS wants an on-the-wire revalidation */
	if (flags & LOOKUP_REVAL)
		goto out_force;
out:
	return (inode->i_nlink == 0) ? -ESTALE : 0;
out_force:
	if (flags & LOOKUP_RCU)
		return -ECHILD;
	ret = __nfs_revalidate_inode(server, inode);
	if (ret != 0)
		return ret;
	goto out;
}