static int tclass_notify(struct sk_buff *oskb, struct nlmsghdr *n,
			  struct Qdisc *q, unsigned long cl, int event)
{
	struct sk_buff *skb;
	u32 pid = oskb ? NETLINK_CB(oskb).pid : 0;

	skb = alloc_skb(NLMSG_GOODSIZE, GFP_KERNEL);
	if (!skb)
		return -ENOBUFS;

	if (tc_fill_tclass(skb, q, cl, pid, n->nlmsg_seq, 0, event) < 0) {
		kfree_skb(skb);
		return -EINVAL;
	}

	return rtnetlink_send(skb, &init_net, pid, RTNLGRP_TC, n->nlmsg_flags&NLM_F_ECHO);
}