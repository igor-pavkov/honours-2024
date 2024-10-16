static void technisat_usb2_green_led_control(struct work_struct *work)
{
	struct technisat_usb2_state *state =
		container_of(work, struct technisat_usb2_state, green_led_work.work);
	struct dvb_frontend *fe = state->dev->adapter[0].fe_adap[0].fe;

	if (state->power_state == 0)
		goto schedule;

	if (fe != NULL) {
		enum fe_status status;

		if (fe->ops.read_status(fe, &status) != 0)
			goto schedule;

		if (status & FE_HAS_LOCK) {
			u32 ber;

			if (fe->ops.read_ber(fe, &ber) != 0)
				goto schedule;

			if (ber > 1000)
				technisat_usb2_set_led(state->dev, 0, TECH_LED_BLINK);
			else
				technisat_usb2_set_led(state->dev, 0, TECH_LED_ON);
		} else
			technisat_usb2_set_led(state->dev, 0, TECH_LED_OFF);
	}

schedule:
	schedule_delayed_work(&state->green_led_work,
			msecs_to_jiffies(500));
}