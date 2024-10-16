static netdev_tx_t yam_send_packet(struct sk_buff *skb,
					 struct net_device *dev)
{
	struct yam_port *yp = netdev_priv(dev);

	skb_queue_tail(&yp->send_queue, skb);
	dev->trans_start = jiffies;
	return NETDEV_TX_OK;
}