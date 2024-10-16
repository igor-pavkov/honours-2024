int cpia2_usb_stream_start(struct camera_data *cam, unsigned int alternate)
{
	int ret;
	int old_alt;

	if(cam->streaming)
		return 0;

	if (cam->flush) {
		int i;
		DBG("Flushing buffers\n");
		for(i=0; i<cam->num_frames; ++i) {
			cam->buffers[i].status = FRAME_EMPTY;
			cam->buffers[i].length = 0;
		}
		cam->curbuff = &cam->buffers[0];
		cam->workbuff = cam->curbuff->next;
		cam->flush = false;
	}

	old_alt = cam->params.camera_state.stream_mode;
	cam->params.camera_state.stream_mode = 0;
	ret = cpia2_usb_change_streaming_alternate(cam, alternate);
	if (ret < 0) {
		int ret2;
		ERR("cpia2_usb_change_streaming_alternate() = %d!\n", ret);
		cam->params.camera_state.stream_mode = old_alt;
		ret2 = set_alternate(cam, USBIF_CMDONLY);
		if (ret2 < 0) {
			ERR("cpia2_usb_change_streaming_alternate(%d) =%d has already failed. Then tried to call set_alternate(USBIF_CMDONLY) = %d.\n",
			    alternate, ret, ret2);
		}
	} else {
		cam->frame_count = 0;
		cam->streaming = 1;
		ret = cpia2_usb_stream_resume(cam);
	}
	return ret;
}