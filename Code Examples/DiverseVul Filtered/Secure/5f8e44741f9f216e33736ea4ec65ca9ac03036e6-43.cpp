static int rtnl_bridge_getlink(struct sk_buff *skb, struct netlink_callback *cb)
{
	struct net *net = sock_net(skb->sk);
	struct net_device *dev;
	int idx = 0;
	u32 portid = NETLINK_CB(cb->skb).portid;
	u32 seq = cb->nlh->nlmsg_seq;
	u32 filter_mask = 0;
	int err;

	if (nlmsg_len(cb->nlh) > sizeof(struct ifinfomsg)) {
		struct nlattr *extfilt;

		extfilt = nlmsg_find_attr(cb->nlh, sizeof(struct ifinfomsg),
					  IFLA_EXT_MASK);
		if (extfilt) {
			if (nla_len(extfilt) < sizeof(filter_mask))
				return -EINVAL;

			filter_mask = nla_get_u32(extfilt);
		}
	}

	rcu_read_lock();
	for_each_netdev_rcu(net, dev) {
		const struct net_device_ops *ops = dev->netdev_ops;
		struct net_device *br_dev = netdev_master_upper_dev_get(dev);

		if (br_dev && br_dev->netdev_ops->ndo_bridge_getlink) {
			if (idx >= cb->args[0]) {
				err = br_dev->netdev_ops->ndo_bridge_getlink(
						skb, portid, seq, dev,
						filter_mask, NLM_F_MULTI);
				if (err < 0 && err != -EOPNOTSUPP)
					break;
			}
			idx++;
		}

		if (ops->ndo_bridge_getlink) {
			if (idx >= cb->args[0]) {
				err = ops->ndo_bridge_getlink(skb, portid,
							      seq, dev,
							      filter_mask,
							      NLM_F_MULTI);
				if (err < 0 && err != -EOPNOTSUPP)
					break;
			}
			idx++;
		}
	}
	rcu_read_unlock();
	cb->args[0] = idx;

	return skb->len;
}