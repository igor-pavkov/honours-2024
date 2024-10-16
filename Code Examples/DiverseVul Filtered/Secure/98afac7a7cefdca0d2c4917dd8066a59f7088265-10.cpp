static int resolve_relative_url(int argc, const char **argv, const char *prefix)
{
	char *remoteurl = NULL;
	char *remote = get_default_remote();
	const char *up_path = NULL;
	char *res;
	const char *url;
	struct strbuf sb = STRBUF_INIT;

	if (argc != 2 && argc != 3)
		die("resolve-relative-url only accepts one or two arguments");

	url = argv[1];
	strbuf_addf(&sb, "remote.%s.url", remote);
	free(remote);

	if (git_config_get_string(sb.buf, &remoteurl))
		/* the repository is its own authoritative upstream */
		remoteurl = xgetcwd();

	if (argc == 3)
		up_path = argv[2];

	res = relative_url(remoteurl, url, up_path);
	puts(res);
	free(res);
	free(remoteurl);
	return 0;
}