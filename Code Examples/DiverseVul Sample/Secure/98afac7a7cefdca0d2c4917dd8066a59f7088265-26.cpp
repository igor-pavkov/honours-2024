static int module_init(int argc, const char **argv, const char *prefix)
{
	struct pathspec pathspec;
	struct module_list list = MODULE_LIST_INIT;
	int quiet = 0;
	int i;

	struct option module_init_options[] = {
		OPT__QUIET(&quiet, N_("Suppress output for initializing a submodule")),
		OPT_END()
	};

	const char *const git_submodule_helper_usage[] = {
		N_("git submodule--helper init [<path>]"),
		NULL
	};

	argc = parse_options(argc, argv, prefix, module_init_options,
			     git_submodule_helper_usage, 0);

	if (module_list_compute(argc, argv, prefix, &pathspec, &list) < 0)
		return 1;

	/*
	 * If there are no path args and submodule.active is set then,
	 * by default, only initialize 'active' modules.
	 */
	if (!argc && git_config_get_value_multi("submodule.active"))
		module_list_active(&list);

	for (i = 0; i < list.nr; i++)
		init_submodule(list.entries[i]->name, prefix, quiet);

	return 0;
}