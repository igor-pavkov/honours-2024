static void vbg_input_close(struct input_dev *input)
{
	struct vbg_dev *gdev = input_get_drvdata(input);

	vbg_core_set_mouse_status(gdev, 0);
}