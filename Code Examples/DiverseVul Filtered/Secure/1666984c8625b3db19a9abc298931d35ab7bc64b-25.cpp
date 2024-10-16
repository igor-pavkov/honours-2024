void usbnet_pause_rx(struct usbnet *dev)
{
	set_bit(EVENT_RX_PAUSED, &dev->flags);

	netif_dbg(dev, rx_status, dev->net, "paused rx queue enabled\n");
}