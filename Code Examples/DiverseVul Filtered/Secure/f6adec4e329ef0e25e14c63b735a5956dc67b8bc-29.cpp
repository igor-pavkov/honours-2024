void submodule_cache_free(struct submodule_cache *cache)
{
	submodule_cache_clear(cache);
	free(cache);
}