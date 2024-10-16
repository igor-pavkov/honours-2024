static int serial_ir_probe(struct platform_device *dev)
{
	struct rc_dev *rcdev;
	int i, nlow, nhigh, result;

	rcdev = devm_rc_allocate_device(&dev->dev, RC_DRIVER_IR_RAW);
	if (!rcdev)
		return -ENOMEM;

	if (hardware[type].send_pulse && hardware[type].send_space)
		rcdev->tx_ir = serial_ir_tx;
	if (hardware[type].set_send_carrier)
		rcdev->s_tx_carrier = serial_ir_tx_carrier;
	if (hardware[type].set_duty_cycle)
		rcdev->s_tx_duty_cycle = serial_ir_tx_duty_cycle;

	switch (type) {
	case IR_HOMEBREW:
		rcdev->device_name = "Serial IR type home-brew";
		break;
	case IR_IRDEO:
		rcdev->device_name = "Serial IR type IRdeo";
		break;
	case IR_IRDEO_REMOTE:
		rcdev->device_name = "Serial IR type IRdeo remote";
		break;
	case IR_ANIMAX:
		rcdev->device_name = "Serial IR type AnimaX";
		break;
	case IR_IGOR:
		rcdev->device_name = "Serial IR type IgorPlug";
		break;
	}

	rcdev->input_phys = KBUILD_MODNAME "/input0";
	rcdev->input_id.bustype = BUS_HOST;
	rcdev->input_id.vendor = 0x0001;
	rcdev->input_id.product = 0x0001;
	rcdev->input_id.version = 0x0100;
	rcdev->open = serial_ir_open;
	rcdev->close = serial_ir_close;
	rcdev->dev.parent = &serial_ir.pdev->dev;
	rcdev->allowed_protocols = RC_PROTO_BIT_ALL_IR_DECODER;
	rcdev->driver_name = KBUILD_MODNAME;
	rcdev->map_name = RC_MAP_RC6_MCE;
	rcdev->min_timeout = 1;
	rcdev->timeout = IR_DEFAULT_TIMEOUT;
	rcdev->max_timeout = 10 * IR_DEFAULT_TIMEOUT;
	rcdev->rx_resolution = 250000;

	serial_ir.rcdev = rcdev;

	timer_setup(&serial_ir.timeout_timer, serial_ir_timeout, 0);

	result = devm_request_irq(&dev->dev, irq, serial_ir_irq_handler,
				  share_irq ? IRQF_SHARED : 0,
				  KBUILD_MODNAME, &hardware);
	if (result < 0) {
		if (result == -EBUSY)
			dev_err(&dev->dev, "IRQ %d busy\n", irq);
		else if (result == -EINVAL)
			dev_err(&dev->dev, "Bad irq number or handler\n");
		return result;
	}

	/* Reserve io region. */
	if ((iommap &&
	     (devm_request_mem_region(&dev->dev, iommap, 8 << ioshift,
				      KBUILD_MODNAME) == NULL)) ||
	     (!iommap && (devm_request_region(&dev->dev, io, 8,
			  KBUILD_MODNAME) == NULL))) {
		dev_err(&dev->dev, "port %04x already in use\n", io);
		dev_warn(&dev->dev, "use 'setserial /dev/ttySX uart none'\n");
		dev_warn(&dev->dev,
			 "or compile the serial port driver as module and\n");
		dev_warn(&dev->dev, "make sure this module is loaded first\n");
		return -EBUSY;
	}

	result = hardware_init_port();
	if (result < 0)
		return result;

	/* Initialize pulse/space widths */
	serial_ir.duty_cycle = 50;
	serial_ir.carrier = 38000;

	/* If pin is high, then this must be an active low receiver. */
	if (sense == -1) {
		/* wait 1/2 sec for the power supply */
		msleep(500);

		/*
		 * probe 9 times every 0.04s, collect "votes" for
		 * active high/low
		 */
		nlow = 0;
		nhigh = 0;
		for (i = 0; i < 9; i++) {
			if (sinp(UART_MSR) & hardware[type].signal_pin)
				nlow++;
			else
				nhigh++;
			msleep(40);
		}
		sense = nlow >= nhigh ? 1 : 0;
		dev_info(&dev->dev, "auto-detected active %s receiver\n",
			 sense ? "low" : "high");
	} else
		dev_info(&dev->dev, "Manually using active %s receiver\n",
			 sense ? "low" : "high");

	dev_dbg(&dev->dev, "Interrupt %d, port %04x obtained\n", irq, io);

	return devm_rc_register_device(&dev->dev, rcdev);
}