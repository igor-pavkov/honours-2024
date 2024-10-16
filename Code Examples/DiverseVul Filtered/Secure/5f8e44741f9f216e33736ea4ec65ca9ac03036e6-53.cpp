static int rtnl_phys_port_name_fill(struct sk_buff *skb, struct net_device *dev)
{
	char name[IFNAMSIZ];
	int err;

	err = dev_get_phys_port_name(dev, name, sizeof(name));
	if (err) {
		if (err == -EOPNOTSUPP)
			return 0;
		return err;
	}

	if (nla_put(skb, IFLA_PHYS_PORT_NAME, strlen(name), name))
		return -EMSGSIZE;

	return 0;
}