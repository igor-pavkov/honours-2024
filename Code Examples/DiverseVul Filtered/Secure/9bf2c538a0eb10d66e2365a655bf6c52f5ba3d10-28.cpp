int set_netnsid_from_name(const char *name, int nsid)
{
	struct {
		struct nlmsghdr n;
		struct rtgenmsg g;
		char            buf[1024];
	} req = {
		.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtgenmsg)),
		.n.nlmsg_flags = NLM_F_REQUEST,
		.n.nlmsg_type = RTM_NEWNSID,
		.g.rtgen_family = AF_UNSPEC,
	};
	int fd, err = 0;

	netns_nsid_socket_init();

	fd = netns_get_fd(name);
	if (fd < 0)
		return fd;

	addattr32(&req.n, 1024, NETNSA_FD, fd);
	addattr32(&req.n, 1024, NETNSA_NSID, nsid);
	if (rtnl_talk(&rth, &req.n, NULL) < 0)
		err = -2;

	close(fd);
	return err;
}