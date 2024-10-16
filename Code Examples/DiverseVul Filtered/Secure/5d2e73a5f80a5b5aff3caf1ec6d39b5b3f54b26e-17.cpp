static int zr364xx_prepare(struct zr364xx_camera *cam)
{
	int res;
	int i, j;

	for (i = 0; init[cam->method][i].size != -1; i++) {
		res = send_control_msg(cam->udev, 1, init[cam->method][i].value,
				     0, init[cam->method][i].bytes,
				     init[cam->method][i].size);
		if (res < 0) {
			dev_err(&cam->udev->dev,
				"error during open sequence: %d\n", i);
			return res;
		}
	}

	cam->skip = 2;
	cam->last_frame = -1;
	cam->cur_frame = 0;
	cam->frame_count = 0;
	for (j = 0; j < FRAMES; j++) {
		cam->buffer.frame[j].ulState = ZR364XX_READ_IDLE;
		cam->buffer.frame[j].cur_size = 0;
	}
	v4l2_ctrl_handler_setup(&cam->ctrl_handler);
	return 0;
}