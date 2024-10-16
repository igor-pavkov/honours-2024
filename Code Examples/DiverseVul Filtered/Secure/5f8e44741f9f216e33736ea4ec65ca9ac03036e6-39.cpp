static int rtnl_dump_ifinfo(struct sk_buff *skb, struct netlink_callback *cb)
{
	struct net *net = sock_net(skb->sk);
	int h, s_h;
	int idx = 0, s_idx;
	struct net_device *dev;
	struct hlist_head *head;
	struct nlattr *tb[IFLA_MAX+1];
	u32 ext_filter_mask = 0;
	const struct rtnl_link_ops *kind_ops = NULL;
	unsigned int flags = NLM_F_MULTI;
	int master_idx = 0;
	int err;
	int hdrlen;

	s_h = cb->args[0];
	s_idx = cb->args[1];

	cb->seq = net->dev_base_seq;

	/* A hack to preserve kernel<->userspace interface.
	 * The correct header is ifinfomsg. It is consistent with rtnl_getlink.
	 * However, before Linux v3.9 the code here assumed rtgenmsg and that's
	 * what iproute2 < v3.9.0 used.
	 * We can detect the old iproute2. Even including the IFLA_EXT_MASK
	 * attribute, its netlink message is shorter than struct ifinfomsg.
	 */
	hdrlen = nlmsg_len(cb->nlh) < sizeof(struct ifinfomsg) ?
		 sizeof(struct rtgenmsg) : sizeof(struct ifinfomsg);

	if (nlmsg_parse(cb->nlh, hdrlen, tb, IFLA_MAX, ifla_policy) >= 0) {

		if (tb[IFLA_EXT_MASK])
			ext_filter_mask = nla_get_u32(tb[IFLA_EXT_MASK]);

		if (tb[IFLA_MASTER])
			master_idx = nla_get_u32(tb[IFLA_MASTER]);

		if (tb[IFLA_LINKINFO])
			kind_ops = linkinfo_to_kind_ops(tb[IFLA_LINKINFO]);

		if (master_idx || kind_ops)
			flags |= NLM_F_DUMP_FILTERED;
	}

	for (h = s_h; h < NETDEV_HASHENTRIES; h++, s_idx = 0) {
		idx = 0;
		head = &net->dev_index_head[h];
		hlist_for_each_entry(dev, head, index_hlist) {
			if (link_dump_filtered(dev, master_idx, kind_ops))
				continue;
			if (idx < s_idx)
				goto cont;
			err = rtnl_fill_ifinfo(skb, dev, RTM_NEWLINK,
					       NETLINK_CB(cb->skb).portid,
					       cb->nlh->nlmsg_seq, 0,
					       flags,
					       ext_filter_mask);
			/* If we ran out of room on the first message,
			 * we're in trouble
			 */
			WARN_ON((err == -EMSGSIZE) && (skb->len == 0));

			if (err < 0)
				goto out;

			nl_dump_check_consistent(cb, nlmsg_hdr(skb));
cont:
			idx++;
		}
	}
out:
	cb->args[1] = idx;
	cb->args[0] = h;

	return skb->len;
}