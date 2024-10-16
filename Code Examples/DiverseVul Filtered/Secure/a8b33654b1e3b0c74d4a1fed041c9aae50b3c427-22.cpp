static int mp_set_info(struct sb_uart_state *state, struct serial_struct *newinfo)
{
	struct serial_struct new_serial;
	struct sb_uart_port *port = state->port;
	unsigned long new_port;
	unsigned int change_irq, change_port, closing_wait;
	unsigned int old_custom_divisor;
	unsigned int old_flags, new_flags;
	int retval = 0;

	if (copy_from_user(&new_serial, newinfo, sizeof(new_serial)))
		return -EFAULT;

	new_port = new_serial.port;
	if (HIGH_BITS_OFFSET)
		new_port += (unsigned long) new_serial.port_high << HIGH_BITS_OFFSET;

	new_serial.irq = irq_canonicalize(new_serial.irq);

	closing_wait = new_serial.closing_wait == ASYNC_CLOSING_WAIT_NONE ?
		USF_CLOSING_WAIT_NONE : new_serial.closing_wait;
	MP_STATE_LOCK(state);

	change_irq  = new_serial.irq != port->irq;
	change_port = new_port != port->iobase ||
		(unsigned long)new_serial.iomem_base != port->mapbase ||
		new_serial.hub6 != port->hub6 ||
		new_serial.io_type != port->iotype ||
		new_serial.iomem_reg_shift != port->regshift ||
		new_serial.type != port->type;
	old_flags = port->flags;
	new_flags = new_serial.flags;
	old_custom_divisor = port->custom_divisor;

	if (!capable(CAP_SYS_ADMIN)) {
		retval = -EPERM;
		if (change_irq || change_port ||
				(new_serial.baud_base != port->uartclk / 16) ||
				(new_serial.close_delay != state->close_delay) ||
				(closing_wait != state->closing_wait) ||
				(new_serial.xmit_fifo_size != port->fifosize) ||
				(((new_flags ^ old_flags) & ~UPF_USR_MASK) != 0))
			goto exit;
		port->flags = ((port->flags & ~UPF_USR_MASK) |
				(new_flags & UPF_USR_MASK));
		port->custom_divisor = new_serial.custom_divisor;
		goto check_and_exit;
	}

	if (port->ops->verify_port)
		retval = port->ops->verify_port(port, &new_serial);

	if ((new_serial.irq >= NR_IRQS) || (new_serial.irq < 0) ||
			(new_serial.baud_base < 9600))
		retval = -EINVAL;

	if (retval)
		goto exit;

	if (change_port || change_irq) {
		retval = -EBUSY;

		if (uart_users(state) > 1)
			goto exit;

		mp_shutdown(state);
	}

	if (change_port) {
		unsigned long old_iobase, old_mapbase;
		unsigned int old_type, old_iotype, old_hub6, old_shift;

		old_iobase = port->iobase;
		old_mapbase = port->mapbase;
		old_type = port->type;
		old_hub6 = port->hub6;
		old_iotype = port->iotype;
		old_shift = port->regshift;

		if (old_type != PORT_UNKNOWN)
			port->ops->release_port(port);

		port->iobase = new_port;
		port->type = new_serial.type;
		port->hub6 = new_serial.hub6;
		port->iotype = new_serial.io_type;
		port->regshift = new_serial.iomem_reg_shift;
		port->mapbase = (unsigned long)new_serial.iomem_base;

		if (port->type != PORT_UNKNOWN) {
			retval = port->ops->request_port(port);
		} else {
			retval = 0;
		}

		if (retval && old_type != PORT_UNKNOWN) {
			port->iobase = old_iobase;
			port->type = old_type;
			port->hub6 = old_hub6;
			port->iotype = old_iotype;
			port->regshift = old_shift;
			port->mapbase = old_mapbase;
			retval = port->ops->request_port(port);
			if (retval)
				port->type = PORT_UNKNOWN;

			retval = -EBUSY;
		}
	}

	port->irq              = new_serial.irq;
	port->uartclk          = new_serial.baud_base * 16;
	port->flags            = (port->flags & ~UPF_CHANGE_MASK) |
		(new_flags & UPF_CHANGE_MASK);
	port->custom_divisor   = new_serial.custom_divisor;
	state->close_delay     = new_serial.close_delay;
	state->closing_wait    = closing_wait;
	port->fifosize         = new_serial.xmit_fifo_size;
	if (state->info->tty)
		state->info->tty->low_latency =
			(port->flags & UPF_LOW_LATENCY) ? 1 : 0;

check_and_exit:
	retval = 0;
	if (port->type == PORT_UNKNOWN)
		goto exit;
	if (state->info->flags & UIF_INITIALIZED) {
		if (((old_flags ^ port->flags) & UPF_SPD_MASK) ||
				old_custom_divisor != port->custom_divisor) {
			if (port->flags & UPF_SPD_MASK) {
				printk(KERN_NOTICE
						"%s sets custom speed on ttyMP%d. This "
						"is deprecated.\n", current->comm,
						port->line);
			}
			mp_change_speed(state, NULL);
		}
	} else
		retval = mp_startup(state, 1);
exit:
	MP_STATE_UNLOCK(state);
	return retval;
}