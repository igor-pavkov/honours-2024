static void usbvision_release(struct usb_usbvision *usbvision)
{
	PDEBUG(DBG_PROBE, "");

	usbvision->initialized = 0;

	usbvision_remove_sysfs(&usbvision->vdev);
	usbvision_unregister_video(usbvision);
	kfree(usbvision->alt_max_pkt_size);

	usb_free_urb(usbvision->ctrl_urb);

	v4l2_ctrl_handler_free(&usbvision->hdl);
	v4l2_device_unregister(&usbvision->v4l2_dev);
	kfree(usbvision);

	PDEBUG(DBG_PROBE, "success");
}