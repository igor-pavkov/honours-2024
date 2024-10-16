int ndo_dflt_fdb_dump(struct sk_buff *skb,
		      struct netlink_callback *cb,
		      struct net_device *dev,
		      int idx)
{
	int err;

	netif_addr_lock_bh(dev);
	err = nlmsg_populate_fdb(skb, cb, dev, &idx, &dev->uc);
	if (err)
		goto out;
	nlmsg_populate_fdb(skb, cb, dev, &idx, &dev->mc);
out:
	netif_addr_unlock_bh(dev);
	return idx;
}