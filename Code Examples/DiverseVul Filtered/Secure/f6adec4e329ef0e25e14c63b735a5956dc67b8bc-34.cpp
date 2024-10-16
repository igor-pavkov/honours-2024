static void cache_put_path(struct submodule_cache *cache,
			   struct submodule *submodule)
{
	unsigned int hash = hash_sha1_string(submodule->gitmodules_sha1,
					     submodule->path);
	struct submodule_entry *e = xmalloc(sizeof(*e));
	hashmap_entry_init(e, hash);
	e->config = submodule;
	hashmap_put(&cache->for_path, e);
}