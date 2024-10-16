int cpia2_usb_stream_resume(struct camera_data *cam)
{
	int ret = 0;
	if(cam->streaming) {
		cam->first_image_seen = 0;
		ret = set_alternate(cam, cam->params.camera_state.stream_mode);
		if(ret == 0) {
			/* for some reason the user effects need to be set
			   again when starting streaming. */
			cpia2_do_command(cam, CPIA2_CMD_SET_USER_EFFECTS, TRANSFER_WRITE,
					cam->params.vp_params.user_effects);
			ret = submit_urbs(cam);
		}
	}
	return ret;
}