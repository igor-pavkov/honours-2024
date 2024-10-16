int cpia2_usb_stream_pause(struct camera_data *cam)
{
	int ret = 0;
	if(cam->streaming) {
		free_sbufs(cam);
		ret = set_alternate(cam, USBIF_CMDONLY);
	}
	return ret;
}