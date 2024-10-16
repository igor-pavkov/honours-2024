static void gitmodules_read_check(struct repository *repo)
{
	submodule_cache_check_init(repo);

	/* read the repo's .gitmodules file if it hasn't been already */
	if (!repo->submodule_cache->gitmodules_read)
		repo_read_gitmodules(repo);
}