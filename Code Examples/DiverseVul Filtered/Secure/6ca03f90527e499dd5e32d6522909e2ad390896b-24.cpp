static int kbd_rate_helper(struct input_handle *handle, void *data)
{
	struct input_dev *dev = handle->dev;
	struct kbd_repeat *rpt = data;

	if (test_bit(EV_REP, dev->evbit)) {

		if (rpt[0].delay > 0)
			input_inject_event(handle,
					   EV_REP, REP_DELAY, rpt[0].delay);
		if (rpt[0].period > 0)
			input_inject_event(handle,
					   EV_REP, REP_PERIOD, rpt[0].period);

		rpt[1].delay = dev->rep[REP_DELAY];
		rpt[1].period = dev->rep[REP_PERIOD];
	}

	return 0;
}