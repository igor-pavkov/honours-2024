static int vbg_input_open(struct input_dev *input)
{
	struct vbg_dev *gdev = input_get_drvdata(input);
	u32 feat = VMMDEV_MOUSE_GUEST_CAN_ABSOLUTE | VMMDEV_MOUSE_NEW_PROTOCOL;
	int ret;

	ret = vbg_core_set_mouse_status(gdev, feat);
	if (ret)
		return ret;

	return 0;
}