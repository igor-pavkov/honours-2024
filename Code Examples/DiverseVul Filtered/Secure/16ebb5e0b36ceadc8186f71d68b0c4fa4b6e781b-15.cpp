static int tc_get_qdisc(struct sk_buff *skb, struct nlmsghdr *n, void *arg)
{
	struct net *net = sock_net(skb->sk);
	struct tcmsg *tcm = NLMSG_DATA(n);
	struct nlattr *tca[TCA_MAX + 1];
	struct net_device *dev;
	u32 clid = tcm->tcm_parent;
	struct Qdisc *q = NULL;
	struct Qdisc *p = NULL;
	int err;

	if (net != &init_net)
		return -EINVAL;

	if ((dev = __dev_get_by_index(&init_net, tcm->tcm_ifindex)) == NULL)
		return -ENODEV;

	err = nlmsg_parse(n, sizeof(*tcm), tca, TCA_MAX, NULL);
	if (err < 0)
		return err;

	if (clid) {
		if (clid != TC_H_ROOT) {
			if (TC_H_MAJ(clid) != TC_H_MAJ(TC_H_INGRESS)) {
				if ((p = qdisc_lookup(dev, TC_H_MAJ(clid))) == NULL)
					return -ENOENT;
				q = qdisc_leaf(p, clid);
			} else { /* ingress */
				q = dev->rx_queue.qdisc_sleeping;
			}
		} else {
			struct netdev_queue *dev_queue;
			dev_queue = netdev_get_tx_queue(dev, 0);
			q = dev_queue->qdisc_sleeping;
		}
		if (!q)
			return -ENOENT;

		if (tcm->tcm_handle && q->handle != tcm->tcm_handle)
			return -EINVAL;
	} else {
		if ((q = qdisc_lookup(dev, tcm->tcm_handle)) == NULL)
			return -ENOENT;
	}

	if (tca[TCA_KIND] && nla_strcmp(tca[TCA_KIND], q->ops->id))
		return -EINVAL;

	if (n->nlmsg_type == RTM_DELQDISC) {
		if (!clid)
			return -EINVAL;
		if (q->handle == 0)
			return -ENOENT;
		if ((err = qdisc_graft(dev, p, skb, n, clid, NULL, q)) != 0)
			return err;
	} else {
		qdisc_notify(skb, n, clid, NULL, q);
	}
	return 0;
}