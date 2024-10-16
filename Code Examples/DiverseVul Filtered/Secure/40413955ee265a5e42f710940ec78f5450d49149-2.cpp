int cipso_v4_cache_add(const unsigned char *cipso_ptr,
		       const struct netlbl_lsm_secattr *secattr)
{
	int ret_val = -EPERM;
	u32 bkt;
	struct cipso_v4_map_cache_entry *entry = NULL;
	struct cipso_v4_map_cache_entry *old_entry = NULL;
	u32 cipso_ptr_len;

	if (!cipso_v4_cache_enabled || cipso_v4_cache_bucketsize <= 0)
		return 0;

	cipso_ptr_len = cipso_ptr[1];

	entry = kzalloc(sizeof(*entry), GFP_ATOMIC);
	if (!entry)
		return -ENOMEM;
	entry->key = kmemdup(cipso_ptr, cipso_ptr_len, GFP_ATOMIC);
	if (!entry->key) {
		ret_val = -ENOMEM;
		goto cache_add_failure;
	}
	entry->key_len = cipso_ptr_len;
	entry->hash = cipso_v4_map_cache_hash(cipso_ptr, cipso_ptr_len);
	refcount_inc(&secattr->cache->refcount);
	entry->lsm_data = secattr->cache;

	bkt = entry->hash & (CIPSO_V4_CACHE_BUCKETS - 1);
	spin_lock_bh(&cipso_v4_cache[bkt].lock);
	if (cipso_v4_cache[bkt].size < cipso_v4_cache_bucketsize) {
		list_add(&entry->list, &cipso_v4_cache[bkt].list);
		cipso_v4_cache[bkt].size += 1;
	} else {
		old_entry = list_entry(cipso_v4_cache[bkt].list.prev,
				       struct cipso_v4_map_cache_entry, list);
		list_del(&old_entry->list);
		list_add(&entry->list, &cipso_v4_cache[bkt].list);
		cipso_v4_cache_entry_free(old_entry);
	}
	spin_unlock_bh(&cipso_v4_cache[bkt].lock);

	return 0;

cache_add_failure:
	if (entry)
		cipso_v4_cache_entry_free(entry);
	return ret_val;
}