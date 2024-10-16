void rtmsg_ifinfo_send(struct sk_buff *skb, struct net_device *dev, gfp_t flags)
{
	struct net *net = dev_net(dev);

	rtnl_notify(skb, net, 0, RTNLGRP_LINK, NULL, flags);
}