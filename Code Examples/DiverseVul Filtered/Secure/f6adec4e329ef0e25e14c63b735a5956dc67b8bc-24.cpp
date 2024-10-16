void submodule_free(void)
{
	if (the_repository->submodule_cache)
		submodule_cache_clear(the_repository->submodule_cache);
}