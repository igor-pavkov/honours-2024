static int cipso_v4_cache_check(const unsigned char *key,
				u32 key_len,
				struct netlbl_lsm_secattr *secattr)
{
	u32 bkt;
	struct cipso_v4_map_cache_entry *entry;
	struct cipso_v4_map_cache_entry *prev_entry = NULL;
	u32 hash;

	if (!cipso_v4_cache_enabled)
		return -ENOENT;

	hash = cipso_v4_map_cache_hash(key, key_len);
	bkt = hash & (CIPSO_V4_CACHE_BUCKETS - 1);
	spin_lock_bh(&cipso_v4_cache[bkt].lock);
	list_for_each_entry(entry, &cipso_v4_cache[bkt].list, list) {
		if (entry->hash == hash &&
		    entry->key_len == key_len &&
		    memcmp(entry->key, key, key_len) == 0) {
			entry->activity += 1;
			refcount_inc(&entry->lsm_data->refcount);
			secattr->cache = entry->lsm_data;
			secattr->flags |= NETLBL_SECATTR_CACHE;
			secattr->type = NETLBL_NLTYPE_CIPSOV4;
			if (!prev_entry) {
				spin_unlock_bh(&cipso_v4_cache[bkt].lock);
				return 0;
			}

			if (prev_entry->activity > 0)
				prev_entry->activity -= 1;
			if (entry->activity > prev_entry->activity &&
			    entry->activity - prev_entry->activity >
			    CIPSO_V4_CACHE_REORDERLIMIT) {
				__list_del(entry->list.prev, entry->list.next);
				__list_add(&entry->list,
					   prev_entry->list.prev,
					   &prev_entry->list);
			}

			spin_unlock_bh(&cipso_v4_cache[bkt].lock);
			return 0;
		}
		prev_entry = entry;
	}
	spin_unlock_bh(&cipso_v4_cache[bkt].lock);

	return -ENOENT;
}