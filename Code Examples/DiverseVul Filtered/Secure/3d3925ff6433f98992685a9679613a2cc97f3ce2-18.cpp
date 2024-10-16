static int usb_8dev_probe(struct usb_interface *intf,
			 const struct usb_device_id *id)
{
	struct net_device *netdev;
	struct usb_8dev_priv *priv;
	int i, err = -ENOMEM;
	u32 version;
	char buf[18];
	struct usb_device *usbdev = interface_to_usbdev(intf);

	/* product id looks strange, better we also check iProduct string */
	if (usb_string(usbdev, usbdev->descriptor.iProduct, buf,
		       sizeof(buf)) > 0 && strcmp(buf, "USB2CAN converter")) {
		dev_info(&usbdev->dev, "ignoring: not an USB2CAN converter\n");
		return -ENODEV;
	}

	netdev = alloc_candev(sizeof(struct usb_8dev_priv), MAX_TX_URBS);
	if (!netdev) {
		dev_err(&intf->dev, "Couldn't alloc candev\n");
		return -ENOMEM;
	}

	priv = netdev_priv(netdev);

	priv->udev = usbdev;
	priv->netdev = netdev;

	priv->can.state = CAN_STATE_STOPPED;
	priv->can.clock.freq = USB_8DEV_ABP_CLOCK;
	priv->can.bittiming_const = &usb_8dev_bittiming_const;
	priv->can.do_set_mode = usb_8dev_set_mode;
	priv->can.do_get_berr_counter = usb_8dev_get_berr_counter;
	priv->can.ctrlmode_supported = CAN_CTRLMODE_LOOPBACK |
				      CAN_CTRLMODE_LISTENONLY |
				      CAN_CTRLMODE_ONE_SHOT |
				      CAN_CTRLMODE_CC_LEN8_DLC;

	netdev->netdev_ops = &usb_8dev_netdev_ops;

	netdev->flags |= IFF_ECHO; /* we support local echo */

	init_usb_anchor(&priv->rx_submitted);

	init_usb_anchor(&priv->tx_submitted);
	atomic_set(&priv->active_tx_urbs, 0);

	for (i = 0; i < MAX_TX_URBS; i++)
		priv->tx_contexts[i].echo_index = MAX_TX_URBS;

	priv->cmd_msg_buffer = devm_kzalloc(&intf->dev, sizeof(struct usb_8dev_cmd_msg),
					    GFP_KERNEL);
	if (!priv->cmd_msg_buffer)
		goto cleanup_candev;

	usb_set_intfdata(intf, priv);

	SET_NETDEV_DEV(netdev, &intf->dev);

	mutex_init(&priv->usb_8dev_cmd_lock);

	err = register_candev(netdev);
	if (err) {
		netdev_err(netdev,
			"couldn't register CAN device: %d\n", err);
		goto cleanup_candev;
	}

	err = usb_8dev_cmd_version(priv, &version);
	if (err) {
		netdev_err(netdev, "can't get firmware version\n");
		goto cleanup_unregister_candev;
	} else {
		netdev_info(netdev,
			 "firmware: %d.%d, hardware: %d.%d\n",
			 (version>>24) & 0xff, (version>>16) & 0xff,
			 (version>>8) & 0xff, version & 0xff);
	}

	devm_can_led_init(netdev);

	return 0;

cleanup_unregister_candev:
	unregister_netdev(priv->netdev);

cleanup_candev:
	free_candev(netdev);

	return err;

}