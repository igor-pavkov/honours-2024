static int module_list(int argc, const char **argv, const char *prefix)
{
	int i;
	struct pathspec pathspec;
	struct module_list list = MODULE_LIST_INIT;

	struct option module_list_options[] = {
		OPT_STRING(0, "prefix", &prefix,
			   N_("path"),
			   N_("alternative anchor for relative paths")),
		OPT_END()
	};

	const char *const git_submodule_helper_usage[] = {
		N_("git submodule--helper list [--prefix=<path>] [<path>...]"),
		NULL
	};

	argc = parse_options(argc, argv, prefix, module_list_options,
			     git_submodule_helper_usage, 0);

	if (module_list_compute(argc, argv, prefix, &pathspec, &list) < 0)
		return 1;

	for (i = 0; i < list.nr; i++) {
		const struct cache_entry *ce = list.entries[i];

		if (ce_stage(ce))
			printf("%06o %s U\t", ce->ce_mode, sha1_to_hex(null_sha1));
		else
			printf("%06o %s %d\t", ce->ce_mode,
			       oid_to_hex(&ce->oid), ce_stage(ce));

		fprintf(stdout, "%s\n", ce->name);
	}
	return 0;
}