static void nfs_mark_dir_for_revalidate(struct inode *inode)
{
	spin_lock(&inode->i_lock);
	nfs_set_cache_invalid(inode, NFS_INO_INVALID_CHANGE);
	spin_unlock(&inode->i_lock);
}