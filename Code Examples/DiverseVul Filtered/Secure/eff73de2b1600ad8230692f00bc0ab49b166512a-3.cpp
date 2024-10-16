int cpia2_usb_change_streaming_alternate(struct camera_data *cam,
					 unsigned int alt)
{
	int ret = 0;

	if(alt < USBIF_ISO_1 || alt > USBIF_ISO_6)
		return -EINVAL;

	if(alt == cam->params.camera_state.stream_mode)
		return 0;

	cpia2_usb_stream_pause(cam);

	configure_transfer_mode(cam, alt);

	cam->params.camera_state.stream_mode = alt;

	/* Reset the camera to prevent image quality degradation */
	cpia2_reset_camera(cam);

	cpia2_usb_stream_resume(cam);

	return ret;
}