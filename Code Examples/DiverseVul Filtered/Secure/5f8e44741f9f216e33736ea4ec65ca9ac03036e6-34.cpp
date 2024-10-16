static int rtnl_bridge_notify(struct net_device *dev)
{
	struct net *net = dev_net(dev);
	struct sk_buff *skb;
	int err = -EOPNOTSUPP;

	if (!dev->netdev_ops->ndo_bridge_getlink)
		return 0;

	skb = nlmsg_new(bridge_nlmsg_size(), GFP_ATOMIC);
	if (!skb) {
		err = -ENOMEM;
		goto errout;
	}

	err = dev->netdev_ops->ndo_bridge_getlink(skb, 0, 0, dev, 0, 0);
	if (err < 0)
		goto errout;

	if (!skb->len)
		goto errout;

	rtnl_notify(skb, net, 0, RTNLGRP_LINK, NULL, GFP_ATOMIC);
	return 0;
errout:
	WARN_ON(err == -EMSGSIZE);
	kfree_skb(skb);
	if (err)
		rtnl_set_sk_err(net, RTNLGRP_LINK, err);
	return err;
}