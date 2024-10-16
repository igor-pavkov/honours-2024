static int usbvision_radio_close(struct file *file)
{
	struct usb_usbvision *usbvision = video_drvdata(file);

	PDEBUG(DBG_IO, "");

	mutex_lock(&usbvision->v4l2_lock);
	/* Set packet size to 0 */
	usbvision->iface_alt = 0;
	usb_set_interface(usbvision->dev, usbvision->iface,
				    usbvision->iface_alt);

	usbvision_audio_off(usbvision);
	usbvision->radio = 0;
	usbvision->user--;

	if (usbvision->remove_pending) {
		printk(KERN_INFO "%s: Final disconnect\n", __func__);
		v4l2_fh_release(file);
		usbvision_release(usbvision);
		return 0;
	}

	mutex_unlock(&usbvision->v4l2_lock);
	PDEBUG(DBG_IO, "success");
	return v4l2_fh_release(file);
}