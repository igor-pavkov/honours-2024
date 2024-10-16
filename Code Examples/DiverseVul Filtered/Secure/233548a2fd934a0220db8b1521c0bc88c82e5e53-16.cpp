void uvc_delete(struct kref *kref)
{
	struct uvc_device *dev = container_of(kref, struct uvc_device, kref);
	struct list_head *p, *n;

	/* Unregister the video device */
	uvc_unregister_video(dev);
	usb_put_intf(dev->intf);
	usb_put_dev(dev->udev);

	uvc_status_cleanup(dev);
	uvc_ctrl_cleanup_device(dev);

	list_for_each_safe(p, n, &dev->entities) {
		struct uvc_entity *entity;
		entity = list_entry(p, struct uvc_entity, list);
		kfree(entity);
	}

	list_for_each_safe(p, n, &dev->streaming) {
		struct uvc_streaming *streaming;
		streaming = list_entry(p, struct uvc_streaming, list);
		usb_driver_release_interface(&uvc_driver.driver,
			streaming->intf);
		usb_put_intf(streaming->intf);
		kfree(streaming->format);
		kfree(streaming->header.bmaControls);
		kfree(streaming);
	}

	kfree(dev);
}