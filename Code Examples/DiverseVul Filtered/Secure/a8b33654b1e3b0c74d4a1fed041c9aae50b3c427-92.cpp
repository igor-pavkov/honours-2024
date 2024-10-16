static int mp_wait_modem_status(struct sb_uart_state *state, unsigned long arg)
{
	struct sb_uart_port *port = state->port;
	DECLARE_WAITQUEUE(wait, current);
	struct sb_uart_icount cprev, cnow;
	int ret;

	spin_lock_irq(&port->lock);
	memcpy(&cprev, &port->icount, sizeof(struct sb_uart_icount));

	port->ops->enable_ms(port);
	spin_unlock_irq(&port->lock);

	add_wait_queue(&state->info->delta_msr_wait, &wait);
	for (;;) {
		spin_lock_irq(&port->lock);
		memcpy(&cnow, &port->icount, sizeof(struct sb_uart_icount));
		spin_unlock_irq(&port->lock);

		set_current_state(TASK_INTERRUPTIBLE);

		if (((arg & TIOCM_RNG) && (cnow.rng != cprev.rng)) ||
				((arg & TIOCM_DSR) && (cnow.dsr != cprev.dsr)) ||
				((arg & TIOCM_CD)  && (cnow.dcd != cprev.dcd)) ||
				((arg & TIOCM_CTS) && (cnow.cts != cprev.cts))) {
			ret = 0;
			break;
		}

		schedule();

		if (signal_pending(current)) {
			ret = -ERESTARTSYS;
			break;
		}

		cprev = cnow;
	}

	current->state = TASK_RUNNING;
	remove_wait_queue(&state->info->delta_msr_wait, &wait);

	return ret;
}