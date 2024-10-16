static int resolve_remote_submodule_branch(int argc, const char **argv,
		const char *prefix)
{
	const char *ret;
	struct strbuf sb = STRBUF_INIT;
	if (argc != 2)
		die("submodule--helper remote-branch takes exactly one arguments, got %d", argc);

	ret = remote_submodule_branch(argv[1]);
	if (!ret)
		die("submodule %s doesn't exist", argv[1]);

	printf("%s", ret);
	strbuf_release(&sb);
	return 0;
}