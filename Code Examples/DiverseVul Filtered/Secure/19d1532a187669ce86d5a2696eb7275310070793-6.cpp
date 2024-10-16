static netdev_tx_t sp_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct sixpack *sp = netdev_priv(dev);

	if (skb->protocol == htons(ETH_P_IP))
		return ax25_ip_xmit(skb);

	spin_lock_bh(&sp->lock);
	/* We were not busy, so we are now... :-) */
	netif_stop_queue(dev);
	dev->stats.tx_bytes += skb->len;
	sp_encaps(sp, skb->data, skb->len);
	spin_unlock_bh(&sp->lock);

	dev_kfree_skb(skb);

	return NETDEV_TX_OK;
}