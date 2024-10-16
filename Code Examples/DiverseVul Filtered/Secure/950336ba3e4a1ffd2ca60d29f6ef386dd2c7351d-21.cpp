static void ati_remote2_input_key(struct ati_remote2 *ar2)
{
	struct input_dev *idev = ar2->idev;
	u8 *data = ar2->buf[1];
	int channel, mode, hw_code, index;

	channel = data[0] >> 4;

	if (!((1 << channel) & ar2->channel_mask))
		return;

	mode = data[0] & 0x0F;

	if (mode > ATI_REMOTE2_PC) {
		dev_err(&ar2->intf[1]->dev,
			"Unknown mode byte (%02x %02x %02x %02x)\n",
			data[3], data[2], data[1], data[0]);
		return;
	}

	hw_code = data[2];
	if (hw_code == 0x3f) {
		/*
		 * For some incomprehensible reason the mouse pad generates
		 * events which look identical to the events from the last
		 * pressed mode key. Naturally we don't want to generate key
		 * events for the mouse pad so we filter out any subsequent
		 * events from the same mode key.
		 */
		if (ar2->mode == mode)
			return;

		if (data[1] == 0)
			ar2->mode = mode;
	}

	if (!((1 << mode) & ar2->mode_mask))
		return;

	index = ati_remote2_lookup(hw_code);
	if (index < 0) {
		dev_err(&ar2->intf[1]->dev,
			"Unknown code byte (%02x %02x %02x %02x)\n",
			data[3], data[2], data[1], data[0]);
		return;
	}

	switch (data[1]) {
	case 0:	/* release */
		break;
	case 1:	/* press */
		ar2->jiffies = jiffies + msecs_to_jiffies(idev->rep[REP_DELAY]);
		break;
	case 2:	/* repeat */

		/* No repeat for mouse buttons. */
		if (ar2->keycode[mode][index] == BTN_LEFT ||
		    ar2->keycode[mode][index] == BTN_RIGHT)
			return;

		if (!time_after_eq(jiffies, ar2->jiffies))
			return;

		ar2->jiffies = jiffies + msecs_to_jiffies(idev->rep[REP_PERIOD]);
		break;
	default:
		dev_err(&ar2->intf[1]->dev,
			"Unknown state byte (%02x %02x %02x %02x)\n",
			data[3], data[2], data[1], data[0]);
		return;
	}

	input_event(idev, EV_KEY, ar2->keycode[mode][index], data[1]);
	input_sync(idev);
}