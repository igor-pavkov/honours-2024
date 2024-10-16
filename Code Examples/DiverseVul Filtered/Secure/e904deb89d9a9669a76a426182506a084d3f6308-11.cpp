static int gitmodules_cb(const char *var, const char *value, void *data)
{
	struct repository *repo = data;
	struct parse_config_parameter parameter;

	parameter.cache = repo->submodule_cache;
	parameter.treeish_name = NULL;
	parameter.gitmodules_sha1 = null_sha1;
	parameter.overwrite = 1;

	return parse_config(var, value, &parameter);
}