void nfs_access_add_cache(struct inode *inode, struct nfs_access_entry *set,
			  const struct cred *cred)
{
	struct nfs_access_entry *cache = kmalloc(sizeof(*cache), GFP_KERNEL);
	if (cache == NULL)
		return;
	RB_CLEAR_NODE(&cache->rb_node);
	cache->fsuid = cred->fsuid;
	cache->fsgid = cred->fsgid;
	cache->group_info = get_group_info(cred->group_info);
	cache->mask = set->mask;

	/* The above field assignments must be visible
	 * before this item appears on the lru.  We cannot easily
	 * use rcu_assign_pointer, so just force the memory barrier.
	 */
	smp_wmb();
	nfs_access_add_rbtree(inode, cache, cred);

	/* Update accounting */
	smp_mb__before_atomic();
	atomic_long_inc(&nfs_access_nr_entries);
	smp_mb__after_atomic();

	/* Add inode to global LRU list */
	if (!test_bit(NFS_INO_ACL_LRU_SET, &NFS_I(inode)->flags)) {
		spin_lock(&nfs_access_lru_lock);
		if (!test_and_set_bit(NFS_INO_ACL_LRU_SET, &NFS_I(inode)->flags))
			list_add_tail(&NFS_I(inode)->access_cache_inode_lru,
					&nfs_access_lru_list);
		spin_unlock(&nfs_access_lru_lock);
	}
	nfs_access_cache_enforce_limit();
}