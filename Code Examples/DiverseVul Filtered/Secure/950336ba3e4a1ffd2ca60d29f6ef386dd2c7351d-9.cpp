static int ati_remote2_input_init(struct ati_remote2 *ar2)
{
	struct input_dev *idev;
	int index, mode, retval;

	idev = input_allocate_device();
	if (!idev)
		return -ENOMEM;

	ar2->idev = idev;
	input_set_drvdata(idev, ar2);

	idev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REP) | BIT_MASK(EV_REL);
	idev->keybit[BIT_WORD(BTN_MOUSE)] = BIT_MASK(BTN_LEFT) |
		BIT_MASK(BTN_RIGHT);
	idev->relbit[0] = BIT_MASK(REL_X) | BIT_MASK(REL_Y);

	for (mode = 0; mode < ATI_REMOTE2_MODES; mode++) {
		for (index = 0; index < ARRAY_SIZE(ati_remote2_key_table); index++) {
			ar2->keycode[mode][index] = ati_remote2_key_table[index].keycode;
			__set_bit(ar2->keycode[mode][index], idev->keybit);
		}
	}

	/* AUX1-AUX4 and PC generate the same scancode. */
	index = ati_remote2_lookup(0x3f);
	ar2->keycode[ATI_REMOTE2_AUX1][index] = KEY_PROG1;
	ar2->keycode[ATI_REMOTE2_AUX2][index] = KEY_PROG2;
	ar2->keycode[ATI_REMOTE2_AUX3][index] = KEY_PROG3;
	ar2->keycode[ATI_REMOTE2_AUX4][index] = KEY_PROG4;
	ar2->keycode[ATI_REMOTE2_PC][index] = KEY_PC;
	__set_bit(KEY_PROG1, idev->keybit);
	__set_bit(KEY_PROG2, idev->keybit);
	__set_bit(KEY_PROG3, idev->keybit);
	__set_bit(KEY_PROG4, idev->keybit);
	__set_bit(KEY_PC, idev->keybit);

	idev->rep[REP_DELAY]  = 250;
	idev->rep[REP_PERIOD] = 33;

	idev->open = ati_remote2_open;
	idev->close = ati_remote2_close;

	idev->getkeycode = ati_remote2_getkeycode;
	idev->setkeycode = ati_remote2_setkeycode;

	idev->name = ar2->name;
	idev->phys = ar2->phys;

	usb_to_input_id(ar2->udev, &idev->id);
	idev->dev.parent = &ar2->udev->dev;

	retval = input_register_device(idev);
	if (retval)
		input_free_device(idev);

	return retval;
}