int cpia2_usb_stream_stop(struct camera_data *cam)
{
	int ret;

	ret = cpia2_usb_stream_pause(cam);
	cam->streaming = 0;
	configure_transfer_mode(cam, 0);
	return ret;
}