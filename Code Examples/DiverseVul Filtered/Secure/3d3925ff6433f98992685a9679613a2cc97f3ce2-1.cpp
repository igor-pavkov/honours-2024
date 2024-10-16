static void usb_8dev_disconnect(struct usb_interface *intf)
{
	struct usb_8dev_priv *priv = usb_get_intfdata(intf);

	usb_set_intfdata(intf, NULL);

	if (priv) {
		netdev_info(priv->netdev, "device disconnected\n");

		unregister_netdev(priv->netdev);
		unlink_all_urbs(priv);
		free_candev(priv->netdev);
	}

}