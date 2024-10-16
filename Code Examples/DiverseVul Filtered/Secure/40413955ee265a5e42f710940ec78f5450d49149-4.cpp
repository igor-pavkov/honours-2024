static int __init cipso_v4_cache_init(void)
{
	u32 iter;

	cipso_v4_cache = kcalloc(CIPSO_V4_CACHE_BUCKETS,
				 sizeof(struct cipso_v4_map_cache_bkt),
				 GFP_KERNEL);
	if (!cipso_v4_cache)
		return -ENOMEM;

	for (iter = 0; iter < CIPSO_V4_CACHE_BUCKETS; iter++) {
		spin_lock_init(&cipso_v4_cache[iter].lock);
		cipso_v4_cache[iter].size = 0;
		INIT_LIST_HEAD(&cipso_v4_cache[iter].list);
	}

	return 0;
}