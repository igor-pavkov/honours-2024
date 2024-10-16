static int tc_dump_tclass_root(struct Qdisc *root, struct sk_buff *skb,
			       struct tcmsg *tcm, struct netlink_callback *cb,
			       int *t_p, int s_t)
{
	struct Qdisc *q;

	if (!root)
		return 0;

	if (tc_dump_tclass_qdisc(root, skb, tcm, cb, t_p, s_t) < 0)
		return -1;

	list_for_each_entry(q, &root->list, list) {
		if (tc_dump_tclass_qdisc(q, skb, tcm, cb, t_p, s_t) < 0)
			return -1;
	}

	return 0;
}