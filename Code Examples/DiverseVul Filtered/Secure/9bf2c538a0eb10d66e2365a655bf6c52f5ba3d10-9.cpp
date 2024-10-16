static int netns_set(int argc, char **argv)
{
	char netns_path[PATH_MAX];
	const char *name;
	int netns, nsid;

	if (argc < 1) {
		fprintf(stderr, "No netns name specified\n");
		return -1;
	}
	if (argc < 2) {
		fprintf(stderr, "No nsid specified\n");
		return -1;
	}
	name = argv[0];
	/* If a negative nsid is specified the kernel will select the nsid. */
	if (strcmp(argv[1], "auto") == 0)
		nsid = -1;
	else if (get_integer(&nsid, argv[1], 0))
		invarg("Invalid \"netnsid\" value\n", argv[1]);
	else if (nsid < 0)
		invarg("\"netnsid\" value should be >= 0\n", argv[1]);

	snprintf(netns_path, sizeof(netns_path), "%s/%s", NETNS_RUN_DIR, name);
	netns = open(netns_path, O_RDONLY | O_CLOEXEC);
	if (netns < 0) {
		fprintf(stderr, "Cannot open network namespace \"%s\": %s\n",
			name, strerror(errno));
		return -1;
	}

	return set_netnsid_from_name(name, nsid);
}