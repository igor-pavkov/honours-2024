static int usb_8dev_set_mode(struct net_device *netdev, enum can_mode mode)
{
	struct usb_8dev_priv *priv = netdev_priv(netdev);
	int err = 0;

	switch (mode) {
	case CAN_MODE_START:
		err = usb_8dev_cmd_open(priv);
		if (err)
			netdev_warn(netdev, "couldn't start device");
		break;

	default:
		return -EOPNOTSUPP;
	}

	return err;
}