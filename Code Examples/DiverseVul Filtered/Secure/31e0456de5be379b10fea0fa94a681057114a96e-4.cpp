static int smsusb_suspend(struct usb_interface *intf, pm_message_t msg)
{
	struct smsusb_device_t *dev = usb_get_intfdata(intf);
	printk(KERN_INFO "%s  Entering status %d.\n", __func__, msg.event);
	dev->state = SMSUSB_SUSPENDED;
	/*smscore_set_power_mode(dev, SMS_POWER_MODE_SUSPENDED);*/
	smsusb_stop_streaming(dev);
	return 0;
}