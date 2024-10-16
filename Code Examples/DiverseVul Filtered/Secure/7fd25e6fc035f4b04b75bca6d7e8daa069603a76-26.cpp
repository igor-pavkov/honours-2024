static int atusb_start(struct ieee802154_hw *hw)
{
	struct atusb *atusb = hw->priv;
	struct usb_device *usb_dev = atusb->usb_dev;
	int ret;

	dev_dbg(&usb_dev->dev, "%s\n", __func__);
	schedule_delayed_work(&atusb->work, 0);
	atusb_command(atusb, ATUSB_RX_MODE, 1);
	ret = atusb_get_and_clear_error(atusb);
	if (ret < 0)
		usb_kill_anchored_urbs(&atusb->idle_urbs);
	return ret;
}