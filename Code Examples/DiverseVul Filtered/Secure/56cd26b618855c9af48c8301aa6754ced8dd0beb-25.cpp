static void frbwrite(unsigned int l, bool is_pulse)
{
	/* simple noise filter */
	static unsigned int ptr, pulse, space;
	struct ir_raw_event ev = {};

	if (ptr > 0 && is_pulse) {
		pulse += l;
		if (pulse > 250000) {
			ev.duration = space;
			ev.pulse = false;
			ir_raw_event_store_with_filter(serial_ir.rcdev, &ev);
			ev.duration = pulse;
			ev.pulse = true;
			ir_raw_event_store_with_filter(serial_ir.rcdev, &ev);
			ptr = 0;
			pulse = 0;
		}
		return;
	}
	if (!is_pulse) {
		if (ptr == 0) {
			if (l > 20000000) {
				space = l;
				ptr++;
				return;
			}
		} else {
			if (l > 20000000) {
				space += pulse;
				if (space > IR_MAX_DURATION)
					space = IR_MAX_DURATION;
				space += l;
				if (space > IR_MAX_DURATION)
					space = IR_MAX_DURATION;
				pulse = 0;
				return;
			}

			ev.duration = space;
			ev.pulse = false;
			ir_raw_event_store_with_filter(serial_ir.rcdev, &ev);
			ev.duration = pulse;
			ev.pulse = true;
			ir_raw_event_store_with_filter(serial_ir.rcdev, &ev);
			ptr = 0;
			pulse = 0;
		}
	}

	ev.duration = l;
	ev.pulse = is_pulse;
	ir_raw_event_store_with_filter(serial_ir.rcdev, &ev);
}