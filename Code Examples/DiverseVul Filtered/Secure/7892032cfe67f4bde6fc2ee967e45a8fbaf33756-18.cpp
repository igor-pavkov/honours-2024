static netdev_tx_t ip6gre_tunnel_xmit(struct sk_buff *skb,
	struct net_device *dev)
{
	struct ip6_tnl *t = netdev_priv(dev);
	struct net_device_stats *stats = &t->dev->stats;
	int ret;

	if (!ip6_tnl_xmit_ctl(t, &t->parms.laddr, &t->parms.raddr))
		goto tx_err;

	switch (skb->protocol) {
	case htons(ETH_P_IP):
		ret = ip6gre_xmit_ipv4(skb, dev);
		break;
	case htons(ETH_P_IPV6):
		ret = ip6gre_xmit_ipv6(skb, dev);
		break;
	default:
		ret = ip6gre_xmit_other(skb, dev);
		break;
	}

	if (ret < 0)
		goto tx_err;

	return NETDEV_TX_OK;

tx_err:
	stats->tx_errors++;
	stats->tx_dropped++;
	kfree_skb(skb);
	return NETDEV_TX_OK;
}