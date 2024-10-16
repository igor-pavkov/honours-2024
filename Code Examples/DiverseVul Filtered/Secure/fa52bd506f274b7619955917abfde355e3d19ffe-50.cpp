static int usbvision_register_video(struct usb_usbvision *usbvision)
{
	int res = -ENOMEM;

	/* Video Device: */
	usbvision_vdev_init(usbvision, &usbvision->vdev,
			      &usbvision_video_template, "USBVision Video");
	if (!usbvision->have_tuner) {
		v4l2_disable_ioctl(&usbvision->vdev, VIDIOC_G_FREQUENCY);
		v4l2_disable_ioctl(&usbvision->vdev, VIDIOC_S_TUNER);
		v4l2_disable_ioctl(&usbvision->vdev, VIDIOC_G_FREQUENCY);
		v4l2_disable_ioctl(&usbvision->vdev, VIDIOC_S_TUNER);
	}
	if (video_register_device(&usbvision->vdev, VFL_TYPE_GRABBER, video_nr) < 0)
		goto err_exit;
	printk(KERN_INFO "USBVision[%d]: registered USBVision Video device %s [v4l2]\n",
	       usbvision->nr, video_device_node_name(&usbvision->vdev));

	/* Radio Device: */
	if (usbvision_device_data[usbvision->dev_model].radio) {
		/* usbvision has radio */
		usbvision_vdev_init(usbvision, &usbvision->rdev,
			      &usbvision_radio_template, "USBVision Radio");
		if (video_register_device(&usbvision->rdev, VFL_TYPE_RADIO, radio_nr) < 0)
			goto err_exit;
		printk(KERN_INFO "USBVision[%d]: registered USBVision Radio device %s [v4l2]\n",
		       usbvision->nr, video_device_node_name(&usbvision->rdev));
	}
	/* all done */
	return 0;

 err_exit:
	dev_err(&usbvision->dev->dev,
		"USBVision[%d]: video_register_device() failed\n",
			usbvision->nr);
	usbvision_unregister_video(usbvision);
	return res;
}