static int zr364xx_s_ctrl(struct v4l2_ctrl *ctrl)
{
	struct zr364xx_camera *cam =
		container_of(ctrl->handler, struct zr364xx_camera, ctrl_handler);
	int temp;

	switch (ctrl->id) {
	case V4L2_CID_BRIGHTNESS:
		/* hardware brightness */
		send_control_msg(cam->udev, 1, 0x2001, 0, NULL, 0);
		temp = (0x60 << 8) + 127 - ctrl->val;
		send_control_msg(cam->udev, 1, temp, 0, NULL, 0);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}