static int nfs_access_get_cached_rcu(struct inode *inode, const struct cred *cred, u32 *mask)
{
	/* Only check the most recently returned cache entry,
	 * but do it without locking.
	 */
	struct nfs_inode *nfsi = NFS_I(inode);
	struct nfs_access_entry *cache;
	int err = -ECHILD;
	struct list_head *lh;

	rcu_read_lock();
	if (nfsi->cache_validity & NFS_INO_INVALID_ACCESS)
		goto out;
	lh = rcu_dereference(list_tail_rcu(&nfsi->access_cache_entry_lru));
	cache = list_entry(lh, struct nfs_access_entry, lru);
	if (lh == &nfsi->access_cache_entry_lru ||
	    access_cmp(cred, cache) != 0)
		cache = NULL;
	if (cache == NULL)
		goto out;
	if (nfs_check_cache_invalid(inode, NFS_INO_INVALID_ACCESS))
		goto out;
	*mask = cache->mask;
	err = 0;
out:
	rcu_read_unlock();
	return err;
}