static int tfilter_notify(struct sk_buff *oskb, struct nlmsghdr *n,
			  struct tcf_proto *tp, unsigned long fh, int event)
{
	struct sk_buff *skb;
	u32 pid = oskb ? NETLINK_CB(oskb).pid : 0;

	skb = alloc_skb(NLMSG_GOODSIZE, GFP_KERNEL);
	if (!skb)
		return -ENOBUFS;

	if (tcf_fill_node(skb, tp, fh, pid, n->nlmsg_seq, 0, event) <= 0) {
		kfree_skb(skb);
		return -EINVAL;
	}

	return rtnetlink_send(skb, &init_net, pid, RTNLGRP_TC,
			      n->nlmsg_flags & NLM_F_ECHO);
}