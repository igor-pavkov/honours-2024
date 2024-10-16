int do_netns(int argc, char **argv)
{
	netns_nsid_socket_init();

	if (argc < 1) {
		netns_map_init();
		return netns_list(0, NULL);
	}

	if (argc > 1 && invalid_name(argv[1])) {
		fprintf(stderr, "Invalid netns name \"%s\"\n", argv[1]);
		exit(-1);
	}

	if ((matches(*argv, "list") == 0) || (matches(*argv, "show") == 0) ||
	    (matches(*argv, "lst") == 0)) {
		netns_map_init();
		return netns_list(argc-1, argv+1);
	}

	if ((matches(*argv, "list-id") == 0)) {
		netns_map_init();
		return netns_list_id(argc-1, argv+1);
	}

	if (matches(*argv, "help") == 0)
		return usage();

	if (matches(*argv, "add") == 0)
		return netns_add(argc-1, argv+1, true);

	if (matches(*argv, "set") == 0)
		return netns_set(argc-1, argv+1);

	if (matches(*argv, "delete") == 0)
		return netns_delete(argc-1, argv+1);

	if (matches(*argv, "identify") == 0)
		return netns_identify(argc-1, argv+1);

	if (matches(*argv, "pids") == 0)
		return netns_pids(argc-1, argv+1);

	if (matches(*argv, "exec") == 0)
		return netns_exec(argc-1, argv+1);

	if (matches(*argv, "monitor") == 0)
		return netns_monitor(argc-1, argv+1);

	if (matches(*argv, "attach") == 0)
		return netns_add(argc-1, argv+1, false);

	fprintf(stderr, "Command \"%s\" is unknown, try \"ip netns help\".\n", *argv);
	exit(-1);
}