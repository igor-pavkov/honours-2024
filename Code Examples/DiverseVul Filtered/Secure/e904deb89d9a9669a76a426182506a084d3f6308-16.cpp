int parse_submodule_fetchjobs(const char *var, const char *value)
{
	int fetchjobs = git_config_int(var, value);
	if (fetchjobs < 0)
		die(_("negative values not allowed for submodule.fetchjobs"));
	return fetchjobs;
}