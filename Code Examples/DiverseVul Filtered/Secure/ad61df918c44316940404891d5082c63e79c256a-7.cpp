static int tcf_node_dump(struct tcf_proto *tp, unsigned long n,
			 struct tcf_walker *arg)
{
	struct tcf_dump_args *a = (void *)arg;

	return tcf_fill_node(a->skb, tp, n, NETLINK_CB(a->cb->skb).pid,
			     a->cb->nlh->nlmsg_seq, NLM_F_MULTI, RTM_NEWTFILTER);
}