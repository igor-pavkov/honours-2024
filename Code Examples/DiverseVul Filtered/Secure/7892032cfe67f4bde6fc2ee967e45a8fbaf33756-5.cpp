static int ip6gre_xmit_other(struct sk_buff *skb, struct net_device *dev)
{
	struct ip6_tnl *t = netdev_priv(dev);
	int encap_limit = -1;
	struct flowi6 fl6;
	__u32 mtu;
	int err;

	if (!(t->parms.flags & IP6_TNL_F_IGN_ENCAP_LIMIT))
		encap_limit = t->parms.encap_limit;

	memcpy(&fl6, &t->fl.u.ip6, sizeof(fl6));

	err = gre_handle_offloads(skb, !!(t->parms.o_flags & TUNNEL_CSUM));
	if (err)
		return err;

	err = __gre6_xmit(skb, dev, 0, &fl6, encap_limit, &mtu, skb->protocol);

	return err;
}