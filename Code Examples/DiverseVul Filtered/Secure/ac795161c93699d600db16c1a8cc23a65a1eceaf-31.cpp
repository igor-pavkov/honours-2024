static void __nfs_access_zap_cache(struct nfs_inode *nfsi, struct list_head *head)
{
	struct rb_root *root_node = &nfsi->access_cache;
	struct rb_node *n;
	struct nfs_access_entry *entry;

	/* Unhook entries from the cache */
	while ((n = rb_first(root_node)) != NULL) {
		entry = rb_entry(n, struct nfs_access_entry, rb_node);
		rb_erase(n, root_node);
		list_move(&entry->lru, head);
	}
	nfsi->cache_validity &= ~NFS_INO_INVALID_ACCESS;
}