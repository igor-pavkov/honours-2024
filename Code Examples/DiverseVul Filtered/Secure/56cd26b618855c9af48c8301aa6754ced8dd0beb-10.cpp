static irqreturn_t serial_ir_irq_handler(int i, void *blah)
{
	ktime_t kt;
	int counter, dcd;
	u8 status;
	ktime_t delkt;
	unsigned int data;
	static int last_dcd = -1;

	if ((sinp(UART_IIR) & UART_IIR_NO_INT)) {
		/* not our interrupt */
		return IRQ_NONE;
	}

	counter = 0;
	do {
		counter++;
		status = sinp(UART_MSR);
		if (counter > RS_ISR_PASS_LIMIT) {
			dev_err(&serial_ir.pdev->dev, "Trapped in interrupt");
			break;
		}
		if ((status & hardware[type].signal_pin_change) &&
		    sense != -1) {
			/* get current time */
			kt = ktime_get();

			/*
			 * The driver needs to know if your receiver is
			 * active high or active low, or the space/pulse
			 * sense could be inverted.
			 */

			/* calc time since last interrupt in nanoseconds */
			dcd = (status & hardware[type].signal_pin) ? 1 : 0;

			if (dcd == last_dcd) {
				dev_err(&serial_ir.pdev->dev,
					"ignoring spike: %d %d %lldns %lldns\n",
					dcd, sense, ktime_to_ns(kt),
					ktime_to_ns(serial_ir.lastkt));
				continue;
			}

			delkt = ktime_sub(kt, serial_ir.lastkt);
			if (ktime_compare(delkt, ktime_set(15, 0)) > 0) {
				data = IR_MAX_DURATION; /* really long time */
				if (!(dcd ^ sense)) {
					/* sanity check */
					dev_err(&serial_ir.pdev->dev,
						"dcd unexpected: %d %d %lldns %lldns\n",
						dcd, sense, ktime_to_ns(kt),
						ktime_to_ns(serial_ir.lastkt));
					/*
					 * detecting pulse while this
					 * MUST be a space!
					 */
					sense = sense ? 0 : 1;
				}
			} else {
				data = ktime_to_ns(delkt);
			}
			frbwrite(data, !(dcd ^ sense));
			serial_ir.lastkt = kt;
			last_dcd = dcd;
		}
	} while (!(sinp(UART_IIR) & UART_IIR_NO_INT)); /* still pending ? */

	mod_timer(&serial_ir.timeout_timer,
		  jiffies + nsecs_to_jiffies(serial_ir.rcdev->timeout));

	ir_raw_event_handle(serial_ir.rcdev);

	return IRQ_HANDLED;
}