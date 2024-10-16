static void uvc_disconnect(struct usb_interface *intf)
{
	struct uvc_device *dev = usb_get_intfdata(intf);

	/* Set the USB interface data to NULL. This can be done outside the
	 * lock, as there's no other reader.
	 */
	usb_set_intfdata(intf, NULL);

	if (intf->cur_altsetting->desc.bInterfaceSubClass == SC_VIDEOSTREAMING)
		return;

	/* uvc_v4l2_open() might race uvc_disconnect(). A static driver-wide
	 * lock is needed to prevent uvc_disconnect from releasing its
	 * reference to the uvc_device instance after uvc_v4l2_open() received
	 * the pointer to the device (video_devdata) but before it got the
	 * chance to increase the reference count (kref_get).
	 */
	mutex_lock(&uvc_driver.open_mutex);

	dev->state |= UVC_DEV_DISCONNECTED;
	kref_put(&dev->kref, uvc_delete);

	mutex_unlock(&uvc_driver.open_mutex);
}