static int xfrm_flush_sa(struct sk_buff *skb, struct nlmsghdr *nlh,
		struct nlattr **attrs)
{
	struct net *net = sock_net(skb->sk);
	struct km_event c;
	struct xfrm_usersa_flush *p = nlmsg_data(nlh);
	int err;

	err = xfrm_state_flush(net, p->proto, true, false);
	if (err) {
		if (err == -ESRCH) /* empty table */
			return 0;
		return err;
	}
	c.data.proto = p->proto;
	c.event = nlh->nlmsg_type;
	c.seq = nlh->nlmsg_seq;
	c.portid = nlh->nlmsg_pid;
	c.net = net;
	km_state_notify(NULL, &c);

	return 0;
}