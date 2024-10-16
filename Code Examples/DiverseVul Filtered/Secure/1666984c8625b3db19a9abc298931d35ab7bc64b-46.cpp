void usbnet_skb_return (struct usbnet *dev, struct sk_buff *skb)
{
	int	status;

	if (test_bit(EVENT_RX_PAUSED, &dev->flags)) {
		skb_queue_tail(&dev->rxq_pause, skb);
		return;
	}

	/* only update if unset to allow minidriver rx_fixup override */
	if (skb->protocol == 0)
		skb->protocol = eth_type_trans (skb, dev->net);

	dev->net->stats.rx_packets++;
	dev->net->stats.rx_bytes += skb->len;

	netif_dbg(dev, rx_status, dev->net, "< rx, len %zu, type 0x%x\n",
		  skb->len + sizeof (struct ethhdr), skb->protocol);
	memset (skb->cb, 0, sizeof (struct skb_data));

	if (skb_defer_rx_timestamp(skb))
		return;

	status = netif_rx (skb);
	if (status != NET_RX_SUCCESS)
		netif_dbg(dev, rx_err, dev->net,
			  "netif_rx status %d\n", status);
}