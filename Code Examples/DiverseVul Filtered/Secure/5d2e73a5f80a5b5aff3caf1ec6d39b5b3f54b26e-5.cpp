static int zr364xx_probe(struct usb_interface *intf,
			 const struct usb_device_id *id)
{
	struct usb_device *udev = interface_to_usbdev(intf);
	struct zr364xx_camera *cam = NULL;
	struct usb_host_interface *iface_desc;
	struct usb_endpoint_descriptor *endpoint;
	struct v4l2_ctrl_handler *hdl;
	int err;
	int i;

	DBG("probing...\n");

	dev_info(&intf->dev, DRIVER_DESC " compatible webcam plugged\n");
	dev_info(&intf->dev, "model %04x:%04x detected\n",
		 le16_to_cpu(udev->descriptor.idVendor),
		 le16_to_cpu(udev->descriptor.idProduct));

	cam = kzalloc(sizeof(*cam), GFP_KERNEL);
	if (!cam)
		return -ENOMEM;

	cam->v4l2_dev.release = zr364xx_release;
	err = v4l2_device_register(&intf->dev, &cam->v4l2_dev);
	if (err < 0) {
		dev_err(&udev->dev, "couldn't register v4l2_device\n");
		kfree(cam);
		return err;
	}
	hdl = &cam->ctrl_handler;
	v4l2_ctrl_handler_init(hdl, 1);
	v4l2_ctrl_new_std(hdl, &zr364xx_ctrl_ops,
			  V4L2_CID_BRIGHTNESS, 0, 127, 1, 64);
	if (hdl->error) {
		err = hdl->error;
		dev_err(&udev->dev, "couldn't register control\n");
		goto fail;
	}
	/* save the init method used by this camera */
	cam->method = id->driver_info;
	mutex_init(&cam->lock);
	cam->vdev = zr364xx_template;
	cam->vdev.lock = &cam->lock;
	cam->vdev.v4l2_dev = &cam->v4l2_dev;
	cam->vdev.ctrl_handler = &cam->ctrl_handler;
	video_set_drvdata(&cam->vdev, cam);

	cam->udev = udev;

	switch (mode) {
	case 1:
		dev_info(&udev->dev, "160x120 mode selected\n");
		cam->width = 160;
		cam->height = 120;
		break;
	case 2:
		dev_info(&udev->dev, "640x480 mode selected\n");
		cam->width = 640;
		cam->height = 480;
		break;
	default:
		dev_info(&udev->dev, "320x240 mode selected\n");
		cam->width = 320;
		cam->height = 240;
		break;
	}

	m0d1[0] = mode;
	m1[2].value = 0xf000 + mode;
	m2[1].value = 0xf000 + mode;

	/* special case for METHOD3, the modes are different */
	if (cam->method == METHOD3) {
		switch (mode) {
		case 1:
			m2[1].value = 0xf000 + 4;
			break;
		case 2:
			m2[1].value = 0xf000 + 0;
			break;
		default:
			m2[1].value = 0xf000 + 1;
			break;
		}
	}

	header2[437] = cam->height / 256;
	header2[438] = cam->height % 256;
	header2[439] = cam->width / 256;
	header2[440] = cam->width % 256;

	cam->nb = 0;

	DBG("dev: %p, udev %p interface %p\n", cam, cam->udev, intf);

	/* set up the endpoint information  */
	iface_desc = intf->cur_altsetting;
	DBG("num endpoints %d\n", iface_desc->desc.bNumEndpoints);
	for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i) {
		endpoint = &iface_desc->endpoint[i].desc;
		if (!cam->read_endpoint && usb_endpoint_is_bulk_in(endpoint)) {
			/* we found the bulk in endpoint */
			cam->read_endpoint = endpoint->bEndpointAddress;
		}
	}

	if (!cam->read_endpoint) {
		err = -ENOMEM;
		dev_err(&intf->dev, "Could not find bulk-in endpoint\n");
		goto fail;
	}

	/* v4l */
	INIT_LIST_HEAD(&cam->vidq.active);
	cam->vidq.cam = cam;

	usb_set_intfdata(intf, cam);

	/* load zr364xx board specific */
	err = zr364xx_board_init(cam);
	if (!err)
		err = v4l2_ctrl_handler_setup(hdl);
	if (err)
		goto fail;

	spin_lock_init(&cam->slock);

	cam->fmt = formats;

	videobuf_queue_vmalloc_init(&cam->vb_vidq, &zr364xx_video_qops,
				    NULL, &cam->slock,
				    V4L2_BUF_TYPE_VIDEO_CAPTURE,
				    V4L2_FIELD_NONE,
				    sizeof(struct zr364xx_buffer), cam, &cam->lock);

	err = video_register_device(&cam->vdev, VFL_TYPE_GRABBER, -1);
	if (err) {
		dev_err(&udev->dev, "video_register_device failed\n");
		goto fail;
	}

	dev_info(&udev->dev, DRIVER_DESC " controlling device %s\n",
		 video_device_node_name(&cam->vdev));
	return 0;

fail:
	v4l2_ctrl_handler_free(hdl);
	v4l2_device_unregister(&cam->v4l2_dev);
	kfree(cam);
	return err;
}