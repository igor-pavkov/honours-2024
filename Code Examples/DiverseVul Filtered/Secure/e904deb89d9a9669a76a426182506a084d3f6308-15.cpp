const struct submodule *submodule_from_path(const struct object_id *treeish_name,
		const char *path)
{
	gitmodules_read_check(the_repository);
	return config_from(the_repository->submodule_cache, treeish_name, path, lookup_path);
}