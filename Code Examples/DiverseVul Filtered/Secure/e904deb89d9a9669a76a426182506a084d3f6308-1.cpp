const struct submodule *submodule_from_name(const struct object_id *treeish_name,
		const char *name)
{
	gitmodules_read_check(the_repository);
	return config_from(the_repository->submodule_cache, treeish_name, name, lookup_name);
}