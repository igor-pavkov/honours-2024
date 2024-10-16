static int netns_list_id(int argc, char **argv)
{
	if (!ipnetns_have_nsid()) {
		fprintf(stderr,
			"RTM_GETNSID is not supported by the kernel.\n");
		return -ENOTSUP;
	}

	if (rtnl_nsiddump_req(&rth, AF_UNSPEC) < 0) {
		perror("Cannot send dump request");
		exit(1);
	}

	new_json_obj(json);
	if (rtnl_dump_filter(&rth, print_nsid, stdout) < 0) {
		delete_json_obj();
		fprintf(stderr, "Dump terminated\n");
		exit(1);
	}
	delete_json_obj();
	return 0;
}