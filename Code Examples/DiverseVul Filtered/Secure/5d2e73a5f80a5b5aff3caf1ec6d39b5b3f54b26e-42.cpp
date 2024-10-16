static int zr364xx_resume(struct usb_interface *intf)
{
	struct zr364xx_camera *cam = usb_get_intfdata(intf);
	int res;

	if (!cam->was_streaming)
		return 0;

	zr364xx_start_readpipe(cam);
	res = zr364xx_prepare(cam);
	if (!res)
		zr364xx_start_acquire(cam);
	return res;
}