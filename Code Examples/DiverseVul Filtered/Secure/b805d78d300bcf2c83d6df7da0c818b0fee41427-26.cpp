static int xfrm_get_spdinfo(struct sk_buff *skb, struct nlmsghdr *nlh,
		struct nlattr **attrs)
{
	struct net *net = sock_net(skb->sk);
	struct sk_buff *r_skb;
	u32 *flags = nlmsg_data(nlh);
	u32 sportid = NETLINK_CB(skb).portid;
	u32 seq = nlh->nlmsg_seq;
	int err;

	r_skb = nlmsg_new(xfrm_spdinfo_msgsize(), GFP_ATOMIC);
	if (r_skb == NULL)
		return -ENOMEM;

	err = build_spdinfo(r_skb, net, sportid, seq, *flags);
	BUG_ON(err < 0);

	return nlmsg_unicast(net->xfrm.nlsk, r_skb, sportid);
}