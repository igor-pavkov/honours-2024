static int on_netns_exec(char *nsname, void *arg)
{
	char **argv = arg;

	cmd_exec(argv[1], argv + 1, true);
	return 0;
}