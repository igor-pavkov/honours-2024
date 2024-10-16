static int rtnl_fdb_dump(struct sk_buff *skb, struct netlink_callback *cb)
{
	struct net_device *dev;
	struct nlattr *tb[IFLA_MAX+1];
	struct net_device *br_dev = NULL;
	const struct net_device_ops *ops = NULL;
	const struct net_device_ops *cops = NULL;
	struct ifinfomsg *ifm = nlmsg_data(cb->nlh);
	struct net *net = sock_net(skb->sk);
	int brport_idx = 0;
	int br_idx = 0;
	int idx = 0;

	if (nlmsg_parse(cb->nlh, sizeof(struct ifinfomsg), tb, IFLA_MAX,
			ifla_policy) == 0) {
		if (tb[IFLA_MASTER])
			br_idx = nla_get_u32(tb[IFLA_MASTER]);
	}

	brport_idx = ifm->ifi_index;

	if (br_idx) {
		br_dev = __dev_get_by_index(net, br_idx);
		if (!br_dev)
			return -ENODEV;

		ops = br_dev->netdev_ops;
	}

	cb->args[1] = 0;
	for_each_netdev(net, dev) {
		if (brport_idx && (dev->ifindex != brport_idx))
			continue;

		if (!br_idx) { /* user did not specify a specific bridge */
			if (dev->priv_flags & IFF_BRIDGE_PORT) {
				br_dev = netdev_master_upper_dev_get(dev);
				cops = br_dev->netdev_ops;
			}

		} else {
			if (dev != br_dev &&
			    !(dev->priv_flags & IFF_BRIDGE_PORT))
				continue;

			if (br_dev != netdev_master_upper_dev_get(dev) &&
			    !(dev->priv_flags & IFF_EBRIDGE))
				continue;

			cops = ops;
		}

		if (dev->priv_flags & IFF_BRIDGE_PORT) {
			if (cops && cops->ndo_fdb_dump)
				idx = cops->ndo_fdb_dump(skb, cb, br_dev, dev,
							 idx);
		}
		if (cb->args[1] == -EMSGSIZE)
			break;

		if (dev->netdev_ops->ndo_fdb_dump)
			idx = dev->netdev_ops->ndo_fdb_dump(skb, cb, dev, NULL,
							    idx);
		else
			idx = ndo_dflt_fdb_dump(skb, cb, dev, NULL, idx);
		if (cb->args[1] == -EMSGSIZE)
			break;

		cops = NULL;
	}

	cb->args[0] = idx;
	return skb->len;
}