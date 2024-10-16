static void cache_remove_path(struct submodule_cache *cache,
			      struct submodule *submodule)
{
	unsigned int hash = hash_sha1_string(submodule->gitmodules_sha1,
					     submodule->path);
	struct submodule_entry e;
	struct submodule_entry *removed;
	hashmap_entry_init(&e, hash);
	e.config = submodule;
	removed = hashmap_remove(&cache->for_path, &e, NULL);
	free(removed);
}