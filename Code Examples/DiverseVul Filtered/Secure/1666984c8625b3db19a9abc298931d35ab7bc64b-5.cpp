void usbnet_resume_rx(struct usbnet *dev)
{
	struct sk_buff *skb;
	int num = 0;

	clear_bit(EVENT_RX_PAUSED, &dev->flags);

	while ((skb = skb_dequeue(&dev->rxq_pause)) != NULL) {
		usbnet_skb_return(dev, skb);
		num++;
	}

	tasklet_schedule(&dev->bh);

	netif_dbg(dev, rx_status, dev->net,
		  "paused rx queue disabled, %d skbs requeued\n", num);
}