struct sk_buff *rtmsg_ifinfo_build_skb(int type, struct net_device *dev,
				       unsigned int change, gfp_t flags)
{
	struct net *net = dev_net(dev);
	struct sk_buff *skb;
	int err = -ENOBUFS;
	size_t if_info_size;

	skb = nlmsg_new((if_info_size = if_nlmsg_size(dev, 0)), flags);
	if (skb == NULL)
		goto errout;

	err = rtnl_fill_ifinfo(skb, dev, type, 0, 0, change, 0, 0);
	if (err < 0) {
		/* -EMSGSIZE implies BUG in if_nlmsg_size() */
		WARN_ON(err == -EMSGSIZE);
		kfree_skb(skb);
		goto errout;
	}
	return skb;
errout:
	if (err < 0)
		rtnl_set_sk_err(net, RTNLGRP_LINK, err);
	return NULL;
}