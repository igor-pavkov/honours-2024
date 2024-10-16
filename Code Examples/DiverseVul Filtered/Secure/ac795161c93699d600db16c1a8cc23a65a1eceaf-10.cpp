static void nfs_drop_nlink(struct inode *inode)
{
	spin_lock(&inode->i_lock);
	/* drop the inode if we're reasonably sure this is the last link */
	if (inode->i_nlink > 0)
		drop_nlink(inode);
	NFS_I(inode)->attr_gencount = nfs_inc_attr_generation_counter();
	nfs_set_cache_invalid(
		inode, NFS_INO_INVALID_CHANGE | NFS_INO_INVALID_CTIME |
			       NFS_INO_INVALID_NLINK);
	spin_unlock(&inode->i_lock);
}