static struct usb_usbvision *usbvision_alloc(struct usb_device *dev,
					     struct usb_interface *intf)
{
	struct usb_usbvision *usbvision;

	usbvision = kzalloc(sizeof(struct usb_usbvision), GFP_KERNEL);
	if (usbvision == NULL)
		return NULL;

	usbvision->dev = dev;
	if (v4l2_device_register(&intf->dev, &usbvision->v4l2_dev))
		goto err_free;

	if (v4l2_ctrl_handler_init(&usbvision->hdl, 4))
		goto err_unreg;
	usbvision->v4l2_dev.ctrl_handler = &usbvision->hdl;
	mutex_init(&usbvision->v4l2_lock);

	/* prepare control urb for control messages during interrupts */
	usbvision->ctrl_urb = usb_alloc_urb(USBVISION_URB_FRAMES, GFP_KERNEL);
	if (usbvision->ctrl_urb == NULL)
		goto err_unreg;
	init_waitqueue_head(&usbvision->ctrl_urb_wq);

	return usbvision;

err_unreg:
	v4l2_ctrl_handler_free(&usbvision->hdl);
	v4l2_device_unregister(&usbvision->v4l2_dev);
err_free:
	kfree(usbvision);
	return NULL;
}