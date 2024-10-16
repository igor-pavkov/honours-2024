static int uvc_register_video(struct uvc_device *dev)
{
	struct video_device *vdev;
	struct uvc_entity *term;
	int found = 0, ret;

	/* Check if the control interface matches the structure we expect. */
	list_for_each_entry(term, &dev->entities, list) {
		struct uvc_streaming *streaming;

		if (UVC_ENTITY_TYPE(term) != TT_STREAMING)
			continue;

		memset(&dev->video, 0, sizeof dev->video);
		mutex_init(&dev->video.ctrl_mutex);
		INIT_LIST_HEAD(&dev->video.iterms);
		INIT_LIST_HEAD(&dev->video.extensions);
		dev->video.oterm = term;
		dev->video.dev = dev;
		if (uvc_scan_chain(&dev->video) < 0)
			continue;

		list_for_each_entry(streaming, &dev->streaming, list) {
			if (streaming->header.bTerminalLink == term->id) {
				dev->video.streaming = streaming;
				found = 1;
				break;
			}
		}

		if (found)
			break;
	}

	if (!found) {
		uvc_printk(KERN_INFO, "No valid video chain found.\n");
		return -1;
	}

	if (uvc_trace_param & UVC_TRACE_PROBE) {
		uvc_printk(KERN_INFO, "Found a valid video chain (");
		list_for_each_entry(term, &dev->video.iterms, chain) {
			printk("%d", term->id);
			if (term->chain.next != &dev->video.iterms)
				printk(",");
		}
		printk(" -> %d).\n", dev->video.oterm->id);
	}

	/* Initialize the streaming interface with default streaming
	 * parameters.
	 */
	if ((ret = uvc_video_init(&dev->video)) < 0) {
		uvc_printk(KERN_ERR, "Failed to initialize the device "
			"(%d).\n", ret);
		return ret;
	}

	/* Register the device with V4L. */
	vdev = video_device_alloc();
	if (vdev == NULL)
		return -1;

	/* We already hold a reference to dev->udev. The video device will be
	 * unregistered before the reference is released, so we don't need to
	 * get another one.
	 */
	vdev->dev = &dev->intf->dev;
	vdev->type = 0;
	vdev->type2 = 0;
	vdev->minor = -1;
	vdev->fops = &uvc_fops;
	vdev->release = video_device_release;
	strncpy(vdev->name, dev->name, sizeof vdev->name);

	/* Set the driver data before calling video_register_device, otherwise
	 * uvc_v4l2_open might race us.
	 *
	 * FIXME: usb_set_intfdata hasn't been called so far. Is that a
	 * 	  problem ? Does any function which could be called here get
	 * 	  a pointer to the usb_interface ?
	 */
	dev->video.vdev = vdev;
	video_set_drvdata(vdev, &dev->video);

	if (video_register_device(vdev, VFL_TYPE_GRABBER, -1) < 0) {
		dev->video.vdev = NULL;
		video_device_release(vdev);
		return -1;
	}

	return 0;
}