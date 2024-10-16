static int cp_event(struct hid_device *hdev, struct hid_field *field,
		struct hid_usage *usage, __s32 value)
{
	unsigned long quirks = (unsigned long)hid_get_drvdata(hdev);

	if (!(hdev->claimed & HID_CLAIMED_INPUT) || !field->hidinput ||
			!usage->type || !(quirks & CP_2WHEEL_MOUSE_HACK))
		return 0;

	if (usage->hid == 0x00090005) {
		if (value)
			quirks |=  CP_2WHEEL_MOUSE_HACK_ON;
		else
			quirks &= ~CP_2WHEEL_MOUSE_HACK_ON;
		hid_set_drvdata(hdev, (void *)quirks);
		return 1;
	}

	if (usage->code == REL_WHEEL && (quirks & CP_2WHEEL_MOUSE_HACK_ON)) {
		struct input_dev *input = field->hidinput->input;

		input_event(input, usage->type, REL_HWHEEL, value);
		return 1;
	}

	return 0;
}