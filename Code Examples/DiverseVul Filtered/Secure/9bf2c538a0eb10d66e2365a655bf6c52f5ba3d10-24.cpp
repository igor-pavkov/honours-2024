int print_nsid(struct nlmsghdr *n, void *arg)
{
	struct rtgenmsg *rthdr = NLMSG_DATA(n);
	struct rtattr *tb[NETNSA_MAX+1];
	int len = n->nlmsg_len;
	FILE *fp = (FILE *)arg;
	struct nsid_cache *c;
	char name[NAME_MAX];
	int nsid;

	if (n->nlmsg_type != RTM_NEWNSID && n->nlmsg_type != RTM_DELNSID)
		return 0;

	len -= NLMSG_SPACE(sizeof(*rthdr));
	if (len < 0) {
		fprintf(stderr, "BUG: wrong nlmsg len %d in %s\n", len,
			__func__);
		return -1;
	}

	parse_rtattr(tb, NETNSA_MAX, NETNS_RTA(rthdr), len);
	if (tb[NETNSA_NSID] == NULL) {
		fprintf(stderr, "BUG: NETNSA_NSID is missing %s\n", __func__);
		return -1;
	}

	open_json_object(NULL);
	if (n->nlmsg_type == RTM_DELNSID)
		print_bool(PRINT_ANY, "deleted", "Deleted ", true);

	nsid = rta_getattr_u32(tb[NETNSA_NSID]);
	print_uint(PRINT_ANY, "nsid", "nsid %u ", nsid);

	c = netns_map_get_by_nsid(nsid);
	if (c != NULL) {
		print_string(PRINT_ANY, "name",
			     "(iproute2 netns name: %s)", c->name);
		netns_map_del(c);
	}

	/* During 'ip monitor nsid', no chance to have new nsid in cache. */
	if (c == NULL && n->nlmsg_type == RTM_NEWNSID)
		if (netns_get_name(nsid, name) == 0) {
			print_string(PRINT_ANY, "name",
				     "(iproute2 netns name: %s)", name);
			netns_map_add(nsid, name);
		}

	print_string(PRINT_FP, NULL, "\n", NULL);
	close_json_object();
	fflush(fp);
	return 0;
}