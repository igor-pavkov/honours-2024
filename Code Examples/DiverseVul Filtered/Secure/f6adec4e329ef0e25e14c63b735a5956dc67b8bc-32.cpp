static void submodule_cache_check_init(struct repository *repo)
{
	if (repo->submodule_cache && repo->submodule_cache->initialized)
		return;

	if (!repo->submodule_cache)
		repo->submodule_cache = submodule_cache_alloc();

	submodule_cache_init(repo->submodule_cache);
}