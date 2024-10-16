static void ims_pcu_gamepad_report(struct ims_pcu *pcu, u32 data)
{
	struct ims_pcu_gamepad *gamepad = pcu->gamepad;
	struct input_dev *input = gamepad->input;
	int x, y;

	x = !!(data & (1 << 14)) - !!(data & (1 << 13));
	y = !!(data & (1 << 12)) - !!(data & (1 << 11));

	input_report_abs(input, ABS_X, x);
	input_report_abs(input, ABS_Y, y);

	input_report_key(input, BTN_A, data & (1 << 7));
	input_report_key(input, BTN_B, data & (1 << 8));
	input_report_key(input, BTN_X, data & (1 << 9));
	input_report_key(input, BTN_Y, data & (1 << 10));
	input_report_key(input, BTN_START, data & (1 << 15));
	input_report_key(input, BTN_SELECT, data & (1 << 16));

	input_sync(input);
}