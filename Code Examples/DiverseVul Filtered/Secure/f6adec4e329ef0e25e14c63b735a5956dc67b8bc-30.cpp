static struct submodule_cache *submodule_cache_alloc(void)
{
	return xcalloc(1, sizeof(struct submodule_cache));
}