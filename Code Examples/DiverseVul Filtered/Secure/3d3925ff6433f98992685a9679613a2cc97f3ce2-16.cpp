static int usb_8dev_close(struct net_device *netdev)
{
	struct usb_8dev_priv *priv = netdev_priv(netdev);
	int err = 0;

	/* Send CLOSE command to CAN controller */
	err = usb_8dev_cmd_close(priv);
	if (err)
		netdev_warn(netdev, "couldn't stop device");

	priv->can.state = CAN_STATE_STOPPED;

	netif_stop_queue(netdev);

	/* Stop polling */
	unlink_all_urbs(priv);

	close_candev(netdev);

	can_led_event(netdev, CAN_LED_EVENT_STOP);

	return err;
}