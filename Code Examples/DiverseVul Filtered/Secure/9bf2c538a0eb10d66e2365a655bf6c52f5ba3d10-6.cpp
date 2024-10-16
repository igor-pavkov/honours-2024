void netns_nsid_socket_init(void)
{
	if (rtnsh.fd > -1 || !ipnetns_have_nsid())
		return;

	if (rtnl_open(&rtnsh, 0) < 0) {
		fprintf(stderr, "Cannot open rtnetlink\n");
		exit(1);
	}

}