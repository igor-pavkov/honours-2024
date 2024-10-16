static struct submodule *cache_lookup_name(struct submodule_cache *cache,
		const unsigned char *gitmodules_sha1, const char *name)
{
	struct submodule_entry *entry;
	unsigned int hash = hash_sha1_string(gitmodules_sha1, name);
	struct submodule_entry key;
	struct submodule key_config;

	hashcpy(key_config.gitmodules_sha1, gitmodules_sha1);
	key_config.name = name;

	hashmap_entry_init(&key, hash);
	key.config = &key_config;

	entry = hashmap_get(&cache->for_name, &key, NULL);
	if (entry)
		return entry->config;
	return NULL;
}