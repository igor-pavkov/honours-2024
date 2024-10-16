static int cpia2_usb_resume(struct usb_interface *intf)
{
	struct camera_data *cam = usb_get_intfdata(intf);

	mutex_lock(&cam->v4l2_lock);
	v4l2_ctrl_handler_setup(&cam->hdl);
	if (cam->streaming) {
		cam->streaming = 0;
		cpia2_usb_stream_start(cam,
				cam->params.camera_state.stream_mode);
	}
	mutex_unlock(&cam->v4l2_lock);

	dev_info(&intf->dev, "coming out of suspend..\n");
	return 0;
}