static int tc_dump_tclass_qdisc(struct Qdisc *q, struct sk_buff *skb,
				struct tcmsg *tcm, struct netlink_callback *cb,
				int *t_p, int s_t)
{
	struct qdisc_dump_args arg;

	if (tc_qdisc_dump_ignore(q) ||
	    *t_p < s_t || !q->ops->cl_ops ||
	    (tcm->tcm_parent &&
	     TC_H_MAJ(tcm->tcm_parent) != q->handle)) {
		(*t_p)++;
		return 0;
	}
	if (*t_p > s_t)
		memset(&cb->args[1], 0, sizeof(cb->args)-sizeof(cb->args[0]));
	arg.w.fn = qdisc_class_dump;
	arg.skb = skb;
	arg.cb = cb;
	arg.w.stop  = 0;
	arg.w.skip = cb->args[1];
	arg.w.count = 0;
	q->ops->cl_ops->walk(q, &arg.w);
	cb->args[1] = arg.w.count;
	if (arg.w.stop)
		return -1;
	(*t_p)++;
	return 0;
}