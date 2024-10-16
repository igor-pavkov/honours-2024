static int xfrm_user_rcv_msg(struct sk_buff *skb, struct nlmsghdr *nlh)
{
	struct net *net = sock_net(skb->sk);
	struct nlattr *attrs[XFRMA_MAX+1];
	const struct xfrm_link *link;
	int type, err;

#ifdef CONFIG_COMPAT
	if (in_compat_syscall())
		return -EOPNOTSUPP;
#endif

	type = nlh->nlmsg_type;
	if (type > XFRM_MSG_MAX)
		return -EINVAL;

	type -= XFRM_MSG_BASE;
	link = &xfrm_dispatch[type];

	/* All operations require privileges, even GET */
	if (!netlink_net_capable(skb, CAP_NET_ADMIN))
		return -EPERM;

	if ((type == (XFRM_MSG_GETSA - XFRM_MSG_BASE) ||
	     type == (XFRM_MSG_GETPOLICY - XFRM_MSG_BASE)) &&
	    (nlh->nlmsg_flags & NLM_F_DUMP)) {
		if (link->dump == NULL)
			return -EINVAL;

		{
			struct netlink_dump_control c = {
				.dump = link->dump,
				.done = link->done,
			};
			return netlink_dump_start(net->xfrm.nlsk, skb, nlh, &c);
		}
	}

	err = nlmsg_parse(nlh, xfrm_msg_min[type], attrs,
			  link->nla_max ? : XFRMA_MAX,
			  link->nla_pol ? : xfrma_policy);
	if (err < 0)
		return err;

	if (link->doit == NULL)
		return -EINVAL;

	return link->doit(skb, nlh, attrs);
}