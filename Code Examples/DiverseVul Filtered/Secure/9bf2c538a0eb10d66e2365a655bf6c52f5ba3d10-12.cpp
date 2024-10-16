static int netns_exec(int argc, char **argv)
{
	/* Setup the proper environment for apps that are not netns
	 * aware, and execute a program in that environment.
	 */
	const char *cmd;

	if (argc < 1 && !do_all) {
		fprintf(stderr, "No netns name specified\n");
		return -1;
	}
	if ((argc < 2 && !do_all) || (argc < 1 && do_all)) {
		fprintf(stderr, "No command specified\n");
		return -1;
	}

	if (do_all)
		return do_each_netns(on_netns_exec, --argv, 1);

	if (netns_switch(argv[0]))
		return -1;

	/* we just changed namespaces. clear any vrf association
	 * with prior namespace before exec'ing command
	 */
	vrf_reset();

	/* ip must return the status of the child,
	 * but do_cmd() will add a minus to this,
	 * so let's add another one here to cancel it.
	 */
	cmd = argv[1];
	return -cmd_exec(cmd, argv + 1, !!batch_mode);
}