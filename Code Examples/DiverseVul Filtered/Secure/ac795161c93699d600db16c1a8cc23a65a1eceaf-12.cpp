void nfs_d_prune_case_insensitive_aliases(struct inode *inode)
{
	/* Case insensitive server? Revalidate dentries */
	if (inode && nfs_server_capable(inode, NFS_CAP_CASE_INSENSITIVE))
		d_prune_aliases(inode);
}