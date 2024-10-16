int usbnet_get_ethernet_addr(struct usbnet *dev, int iMACAddress)
{
	int 		tmp = -1, ret;
	unsigned char	buf [13];

	ret = usb_string(dev->udev, iMACAddress, buf, sizeof buf);
	if (ret == 12)
		tmp = hex2bin(dev->net->dev_addr, buf, 6);
	if (tmp < 0) {
		dev_dbg(&dev->udev->dev,
			"bad MAC string %d fetch, %d\n", iMACAddress, tmp);
		if (ret >= 0)
			ret = -EINVAL;
		return ret;
	}
	return 0;
}