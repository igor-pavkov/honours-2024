const struct submodule *submodule_from_cache(struct repository *repo,
					     const struct object_id *treeish_name,
					     const char *key)
{
	gitmodules_read_check(repo);
	return config_from(repo->submodule_cache, treeish_name,
			   key, lookup_path);
}