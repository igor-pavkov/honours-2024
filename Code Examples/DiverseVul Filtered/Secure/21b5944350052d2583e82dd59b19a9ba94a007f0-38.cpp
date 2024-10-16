static int rtnl_net_fill(struct sk_buff *skb, u32 portid, u32 seq, int flags,
			 int cmd, struct net *net, int nsid)
{
	struct nlmsghdr *nlh;
	struct rtgenmsg *rth;

	nlh = nlmsg_put(skb, portid, seq, cmd, sizeof(*rth), flags);
	if (!nlh)
		return -EMSGSIZE;

	rth = nlmsg_data(nlh);
	rth->rtgen_family = AF_UNSPEC;

	if (nla_put_s32(skb, NETNSA_NSID, nsid))
		goto nla_put_failure;

	nlmsg_end(skb, nlh);
	return 0;

nla_put_failure:
	nlmsg_cancel(skb, nlh);
	return -EMSGSIZE;
}