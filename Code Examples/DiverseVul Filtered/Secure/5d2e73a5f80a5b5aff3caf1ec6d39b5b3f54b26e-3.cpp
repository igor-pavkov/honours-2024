static void zr364xx_disconnect(struct usb_interface *intf)
{
	struct zr364xx_camera *cam = usb_get_intfdata(intf);

	mutex_lock(&cam->lock);
	usb_set_intfdata(intf, NULL);
	dev_info(&intf->dev, DRIVER_DESC " webcam unplugged\n");
	video_unregister_device(&cam->vdev);
	v4l2_device_disconnect(&cam->v4l2_dev);

	/* stops the read pipe if it is running */
	if (cam->b_acquire)
		zr364xx_stop_acquire(cam);

	zr364xx_stop_readpipe(cam);
	mutex_unlock(&cam->lock);
	v4l2_device_put(&cam->v4l2_dev);
}