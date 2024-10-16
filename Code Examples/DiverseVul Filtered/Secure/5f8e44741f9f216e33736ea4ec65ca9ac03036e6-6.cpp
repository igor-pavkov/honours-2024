static int rtnl_group_changelink(const struct sk_buff *skb,
		struct net *net, int group,
		struct ifinfomsg *ifm,
		struct nlattr **tb)
{
	struct net_device *dev, *aux;
	int err;

	for_each_netdev_safe(net, dev, aux) {
		if (dev->group == group) {
			err = do_setlink(skb, dev, ifm, tb, NULL, 0);
			if (err < 0)
				return err;
		}
	}

	return 0;
}