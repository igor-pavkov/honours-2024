static int resolve_relative_path(int argc, const char **argv, const char *prefix)
{
	struct strbuf sb = STRBUF_INIT;
	if (argc != 3)
		die("submodule--helper relative-path takes exactly 2 arguments, got %d", argc);

	printf("%s", relative_path(argv[1], argv[2], &sb));
	strbuf_release(&sb);
	return 0;
}