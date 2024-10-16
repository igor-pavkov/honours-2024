static int nfs_is_exclusive_create(struct inode *dir, unsigned int flags)
{
	if (NFS_PROTO(dir)->version == 2)
		return 0;
	return flags & LOOKUP_EXCL;
}