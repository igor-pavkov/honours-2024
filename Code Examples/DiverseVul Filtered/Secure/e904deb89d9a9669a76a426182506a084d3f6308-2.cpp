static void submodule_cache_init(struct submodule_cache *cache)
{
	hashmap_init(&cache->for_path, config_path_cmp, NULL, 0);
	hashmap_init(&cache->for_name, config_name_cmp, NULL, 0);
	cache->initialized = 1;
}