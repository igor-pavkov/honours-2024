static void usbnet_set_rx_mode(struct net_device *net)
{
	struct usbnet		*dev = netdev_priv(net);

	usbnet_defer_kevent(dev, EVENT_SET_RX_MODE);
}