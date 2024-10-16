static int netns_delete(int argc, char **argv)
{
	if (argc < 1 && !do_all) {
		fprintf(stderr, "No netns name specified\n");
		return -1;
	}

	if (do_all)
		return netns_foreach(on_netns_del, NULL);

	return on_netns_del(argv[0], NULL);
}