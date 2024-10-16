static int tc_dump_tfilter(struct sk_buff *skb, struct netlink_callback *cb)
{
	struct net *net = sock_net(skb->sk);
	int t;
	int s_t;
	struct net_device *dev;
	struct Qdisc *q;
	struct tcf_proto *tp, **chain;
	struct tcmsg *tcm = (struct tcmsg *)NLMSG_DATA(cb->nlh);
	unsigned long cl = 0;
	const struct Qdisc_class_ops *cops;
	struct tcf_dump_args arg;

	if (net != &init_net)
		return 0;

	if (cb->nlh->nlmsg_len < NLMSG_LENGTH(sizeof(*tcm)))
		return skb->len;
	if ((dev = dev_get_by_index(&init_net, tcm->tcm_ifindex)) == NULL)
		return skb->len;

	if (!tcm->tcm_parent)
		q = dev->qdisc;
	else
		q = qdisc_lookup(dev, TC_H_MAJ(tcm->tcm_parent));
	if (!q)
		goto out;
	if ((cops = q->ops->cl_ops) == NULL)
		goto errout;
	if (cops->tcf_chain == NULL)
		goto errout;
	if (TC_H_MIN(tcm->tcm_parent)) {
		cl = cops->get(q, tcm->tcm_parent);
		if (cl == 0)
			goto errout;
	}
	chain = cops->tcf_chain(q, cl);
	if (chain == NULL)
		goto errout;

	s_t = cb->args[0];

	for (tp=*chain, t=0; tp; tp = tp->next, t++) {
		if (t < s_t) continue;
		if (TC_H_MAJ(tcm->tcm_info) &&
		    TC_H_MAJ(tcm->tcm_info) != tp->prio)
			continue;
		if (TC_H_MIN(tcm->tcm_info) &&
		    TC_H_MIN(tcm->tcm_info) != tp->protocol)
			continue;
		if (t > s_t)
			memset(&cb->args[1], 0, sizeof(cb->args)-sizeof(cb->args[0]));
		if (cb->args[1] == 0) {
			if (tcf_fill_node(skb, tp, 0, NETLINK_CB(cb->skb).pid,
					  cb->nlh->nlmsg_seq, NLM_F_MULTI,
					  RTM_NEWTFILTER) <= 0)
				break;

			cb->args[1] = 1;
		}
		if (tp->ops->walk == NULL)
			continue;
		arg.w.fn = tcf_node_dump;
		arg.skb = skb;
		arg.cb = cb;
		arg.w.stop = 0;
		arg.w.skip = cb->args[1]-1;
		arg.w.count = 0;
		tp->ops->walk(tp, &arg.w);
		cb->args[1] = arg.w.count+1;
		if (arg.w.stop)
			break;
	}

	cb->args[0] = t;

errout:
	if (cl)
		cops->put(q, cl);
out:
	dev_put(dev);
	return skb->len;
}