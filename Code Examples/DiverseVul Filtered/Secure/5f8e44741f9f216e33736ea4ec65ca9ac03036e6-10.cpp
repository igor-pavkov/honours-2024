static int rtnl_phys_port_id_fill(struct sk_buff *skb, struct net_device *dev)
{
	int err;
	struct netdev_phys_item_id ppid;

	err = dev_get_phys_port_id(dev, &ppid);
	if (err) {
		if (err == -EOPNOTSUPP)
			return 0;
		return err;
	}

	if (nla_put(skb, IFLA_PHYS_PORT_ID, ppid.id_len, ppid.id))
		return -EMSGSIZE;

	return 0;
}