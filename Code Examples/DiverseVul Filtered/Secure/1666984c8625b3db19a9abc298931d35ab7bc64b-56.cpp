void usbnet_purge_paused_rxq(struct usbnet *dev)
{
	skb_queue_purge(&dev->rxq_pause);
}