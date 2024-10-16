static int zr364xx_suspend(struct usb_interface *intf, pm_message_t message)
{
	struct zr364xx_camera *cam = usb_get_intfdata(intf);

	cam->was_streaming = cam->b_acquire;
	if (!cam->was_streaming)
		return 0;
	zr364xx_stop_acquire(cam);
	zr364xx_stop_readpipe(cam);
	return 0;
}