static int usb_8dev_get_berr_counter(const struct net_device *netdev,
				     struct can_berr_counter *bec)
{
	struct usb_8dev_priv *priv = netdev_priv(netdev);

	bec->txerr = priv->bec.txerr;
	bec->rxerr = priv->bec.rxerr;

	return 0;
}