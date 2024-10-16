void usbnet_get_drvinfo (struct net_device *net, struct ethtool_drvinfo *info)
{
	struct usbnet *dev = netdev_priv(net);

	strlcpy (info->driver, dev->driver_name, sizeof info->driver);
	strlcpy (info->version, DRIVER_VERSION, sizeof info->version);
	strlcpy (info->fw_version, dev->driver_info->description,
		sizeof info->fw_version);
	usb_make_path (dev->udev, info->bus_info, sizeof info->bus_info);
}