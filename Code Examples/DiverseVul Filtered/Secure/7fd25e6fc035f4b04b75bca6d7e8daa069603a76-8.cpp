static void atusb_stop(struct ieee802154_hw *hw)
{
	struct atusb *atusb = hw->priv;
	struct usb_device *usb_dev = atusb->usb_dev;

	dev_dbg(&usb_dev->dev, "%s\n", __func__);
	usb_kill_anchored_urbs(&atusb->idle_urbs);
	atusb_command(atusb, ATUSB_RX_MODE, 0);
	atusb_get_and_clear_error(atusb);
}