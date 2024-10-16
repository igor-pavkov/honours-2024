static int cpia2_usb_suspend(struct usb_interface *intf, pm_message_t message)
{
	struct camera_data *cam = usb_get_intfdata(intf);

	mutex_lock(&cam->v4l2_lock);
	if (cam->streaming) {
		cpia2_usb_stream_stop(cam);
		cam->streaming = 1;
	}
	mutex_unlock(&cam->v4l2_lock);

	dev_info(&intf->dev, "going into suspend..\n");
	return 0;
}