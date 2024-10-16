static void cache_add(struct submodule_cache *cache,
		      struct submodule *submodule)
{
	unsigned int hash = hash_sha1_string(submodule->gitmodules_sha1,
					     submodule->name);
	struct submodule_entry *e = xmalloc(sizeof(*e));
	hashmap_entry_init(e, hash);
	e->config = submodule;
	hashmap_add(&cache->for_name, e);
}