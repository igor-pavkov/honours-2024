static int module_name(int argc, const char **argv, const char *prefix)
{
	const struct submodule *sub;

	if (argc != 2)
		usage(_("git submodule--helper name <path>"));

	gitmodules_config();
	sub = submodule_from_path(null_sha1, argv[1]);

	if (!sub)
		die(_("no submodule mapping found in .gitmodules for path '%s'"),
		    argv[1]);

	printf("%s\n", sub->name);

	return 0;
}