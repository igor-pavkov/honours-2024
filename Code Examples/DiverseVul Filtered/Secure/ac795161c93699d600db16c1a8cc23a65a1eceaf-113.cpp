static int nfs_access_get_cached_locked(struct inode *inode, const struct cred *cred, u32 *mask, bool may_block)
{
	struct nfs_inode *nfsi = NFS_I(inode);
	struct nfs_access_entry *cache;
	bool retry = true;
	int err;

	spin_lock(&inode->i_lock);
	for(;;) {
		if (nfsi->cache_validity & NFS_INO_INVALID_ACCESS)
			goto out_zap;
		cache = nfs_access_search_rbtree(inode, cred);
		err = -ENOENT;
		if (cache == NULL)
			goto out;
		/* Found an entry, is our attribute cache valid? */
		if (!nfs_check_cache_invalid(inode, NFS_INO_INVALID_ACCESS))
			break;
		if (!retry)
			break;
		err = -ECHILD;
		if (!may_block)
			goto out;
		spin_unlock(&inode->i_lock);
		err = __nfs_revalidate_inode(NFS_SERVER(inode), inode);
		if (err)
			return err;
		spin_lock(&inode->i_lock);
		retry = false;
	}
	*mask = cache->mask;
	list_move_tail(&cache->lru, &nfsi->access_cache_entry_lru);
	err = 0;
out:
	spin_unlock(&inode->i_lock);
	return err;
out_zap:
	spin_unlock(&inode->i_lock);
	nfs_access_zap_cache(inode);
	return -ENOENT;
}