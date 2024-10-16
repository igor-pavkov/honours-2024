static int serial_ir_tx(struct rc_dev *dev, unsigned int *txbuf,
			unsigned int count)
{
	unsigned long flags;
	ktime_t edge;
	s64 delta;
	int i;

	spin_lock_irqsave(&hardware[type].lock, flags);
	if (type == IR_IRDEO) {
		/* DTR, RTS down */
		on();
	}

	edge = ktime_get();
	for (i = 0; i < count; i++) {
		if (i % 2)
			hardware[type].send_space();
		else
			hardware[type].send_pulse(txbuf[i], edge);

		edge = ktime_add_us(edge, txbuf[i]);
		delta = ktime_us_delta(edge, ktime_get());
		if (delta > 25) {
			spin_unlock_irqrestore(&hardware[type].lock, flags);
			usleep_range(delta - 25, delta + 25);
			spin_lock_irqsave(&hardware[type].lock, flags);
		} else if (delta > 0) {
			udelay(delta);
		}
	}
	off();
	spin_unlock_irqrestore(&hardware[type].lock, flags);
	return count;
}