static int absorb_git_dirs(int argc, const char **argv, const char *prefix)
{
	int i;
	struct pathspec pathspec;
	struct module_list list = MODULE_LIST_INIT;
	unsigned flags = ABSORB_GITDIR_RECURSE_SUBMODULES;

	struct option embed_gitdir_options[] = {
		OPT_STRING(0, "prefix", &prefix,
			   N_("path"),
			   N_("path into the working tree")),
		OPT_BIT(0, "--recursive", &flags, N_("recurse into submodules"),
			ABSORB_GITDIR_RECURSE_SUBMODULES),
		OPT_END()
	};

	const char *const git_submodule_helper_usage[] = {
		N_("git submodule--helper embed-git-dir [<path>...]"),
		NULL
	};

	argc = parse_options(argc, argv, prefix, embed_gitdir_options,
			     git_submodule_helper_usage, 0);

	gitmodules_config();
	git_config(submodule_config, NULL);

	if (module_list_compute(argc, argv, prefix, &pathspec, &list) < 0)
		return 1;

	for (i = 0; i < list.nr; i++)
		absorb_git_dir_into_superproject(prefix,
				list.entries[i]->name, flags);

	return 0;
}