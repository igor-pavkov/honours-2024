static void rtnl_fdb_notify(struct net_device *dev, u8 *addr, u16 vid, int type,
			    u16 ndm_state)
{
	struct net *net = dev_net(dev);
	struct sk_buff *skb;
	int err = -ENOBUFS;

	skb = nlmsg_new(rtnl_fdb_nlmsg_size(), GFP_ATOMIC);
	if (!skb)
		goto errout;

	err = nlmsg_populate_fdb_fill(skb, dev, addr, vid,
				      0, 0, type, NTF_SELF, 0, ndm_state);
	if (err < 0) {
		kfree_skb(skb);
		goto errout;
	}

	rtnl_notify(skb, net, 0, RTNLGRP_NEIGH, NULL, GFP_ATOMIC);
	return;
errout:
	rtnl_set_sk_err(net, RTNLGRP_NEIGH, err);
}