static int tc_ctl_tclass(struct sk_buff *skb, struct nlmsghdr *n, void *arg)
{
	struct net *net = sock_net(skb->sk);
	struct netdev_queue *dev_queue;
	struct tcmsg *tcm = NLMSG_DATA(n);
	struct nlattr *tca[TCA_MAX + 1];
	struct net_device *dev;
	struct Qdisc *q = NULL;
	const struct Qdisc_class_ops *cops;
	unsigned long cl = 0;
	unsigned long new_cl;
	u32 pid = tcm->tcm_parent;
	u32 clid = tcm->tcm_handle;
	u32 qid = TC_H_MAJ(clid);
	int err;

	if (net != &init_net)
		return -EINVAL;

	if ((dev = __dev_get_by_index(&init_net, tcm->tcm_ifindex)) == NULL)
		return -ENODEV;

	err = nlmsg_parse(n, sizeof(*tcm), tca, TCA_MAX, NULL);
	if (err < 0)
		return err;

	/*
	   parent == TC_H_UNSPEC - unspecified parent.
	   parent == TC_H_ROOT   - class is root, which has no parent.
	   parent == X:0	 - parent is root class.
	   parent == X:Y	 - parent is a node in hierarchy.
	   parent == 0:Y	 - parent is X:Y, where X:0 is qdisc.

	   handle == 0:0	 - generate handle from kernel pool.
	   handle == 0:Y	 - class is X:Y, where X:0 is qdisc.
	   handle == X:Y	 - clear.
	   handle == X:0	 - root class.
	 */

	/* Step 1. Determine qdisc handle X:0 */

	dev_queue = netdev_get_tx_queue(dev, 0);
	if (pid != TC_H_ROOT) {
		u32 qid1 = TC_H_MAJ(pid);

		if (qid && qid1) {
			/* If both majors are known, they must be identical. */
			if (qid != qid1)
				return -EINVAL;
		} else if (qid1) {
			qid = qid1;
		} else if (qid == 0)
			qid = dev_queue->qdisc_sleeping->handle;

		/* Now qid is genuine qdisc handle consistent
		   both with parent and child.

		   TC_H_MAJ(pid) still may be unspecified, complete it now.
		 */
		if (pid)
			pid = TC_H_MAKE(qid, pid);
	} else {
		if (qid == 0)
			qid = dev_queue->qdisc_sleeping->handle;
	}

	/* OK. Locate qdisc */
	if ((q = qdisc_lookup(dev, qid)) == NULL)
		return -ENOENT;

	/* An check that it supports classes */
	cops = q->ops->cl_ops;
	if (cops == NULL)
		return -EINVAL;

	/* Now try to get class */
	if (clid == 0) {
		if (pid == TC_H_ROOT)
			clid = qid;
	} else
		clid = TC_H_MAKE(qid, clid);

	if (clid)
		cl = cops->get(q, clid);

	if (cl == 0) {
		err = -ENOENT;
		if (n->nlmsg_type != RTM_NEWTCLASS || !(n->nlmsg_flags&NLM_F_CREATE))
			goto out;
	} else {
		switch (n->nlmsg_type) {
		case RTM_NEWTCLASS:
			err = -EEXIST;
			if (n->nlmsg_flags&NLM_F_EXCL)
				goto out;
			break;
		case RTM_DELTCLASS:
			err = cops->delete(q, cl);
			if (err == 0)
				tclass_notify(skb, n, q, cl, RTM_DELTCLASS);
			goto out;
		case RTM_GETTCLASS:
			err = tclass_notify(skb, n, q, cl, RTM_NEWTCLASS);
			goto out;
		default:
			err = -EINVAL;
			goto out;
		}
	}

	new_cl = cl;
	err = cops->change(q, clid, pid, tca, &new_cl);
	if (err == 0)
		tclass_notify(skb, n, q, new_cl, RTM_NEWTCLASS);

out:
	if (cl)
		cops->put(q, cl);

	return err;
}