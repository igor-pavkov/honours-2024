static int vbg_create_input_device(struct vbg_dev *gdev)
{
	struct input_dev *input;

	input = devm_input_allocate_device(gdev->dev);
	if (!input)
		return -ENOMEM;

	input->id.bustype = BUS_PCI;
	input->id.vendor = VBOX_VENDORID;
	input->id.product = VMMDEV_DEVICEID;
	input->open = vbg_input_open;
	input->close = vbg_input_close;
	input->dev.parent = gdev->dev;
	input->name = "VirtualBox mouse integration";

	input_set_abs_params(input, ABS_X, VMMDEV_MOUSE_RANGE_MIN,
			     VMMDEV_MOUSE_RANGE_MAX, 0, 0);
	input_set_abs_params(input, ABS_Y, VMMDEV_MOUSE_RANGE_MIN,
			     VMMDEV_MOUSE_RANGE_MAX, 0, 0);
	input_set_capability(input, EV_KEY, BTN_MOUSE);
	input_set_drvdata(input, gdev);

	gdev->input = input;

	return input_register_device(gdev->input);
}