void xfrm_dst_ifdown(struct dst_entry *dst, struct net_device *dev)
{
	while ((dst = dst->child) && dst->xfrm && dst->dev == dev) {
		dst->dev = dev_net(dev)->loopback_dev;
		dev_hold(dst->dev);
		dev_put(dev);
	}
}