void usbnet_link_change(struct usbnet *dev, bool link, bool need_reset)
{
	/* update link after link is reseted */
	if (link && !need_reset)
		netif_carrier_on(dev->net);
	else
		netif_carrier_off(dev->net);

	if (need_reset && link)
		usbnet_defer_kevent(dev, EVENT_LINK_RESET);
	else
		usbnet_defer_kevent(dev, EVENT_LINK_CHANGE);
}