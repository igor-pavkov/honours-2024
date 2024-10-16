static int mp_open(struct tty_struct *tty, struct file *filp)
{
	struct uart_driver *drv = (struct uart_driver *)tty->driver->driver_state;
	struct sb_uart_state *state;
	int retval;
	int  line = tty->index;
	struct mp_port *mtpt;

	retval = -ENODEV;
	if (line >= tty->driver->num)
		goto fail;

	state = uart_get(drv, line);

	if (IS_ERR(state)) {
		retval = PTR_ERR(state);
		goto fail;
	}

	mtpt  = (struct mp_port *)state->port;

	tty->driver_data = state;
	tty->low_latency = (state->port->flags & UPF_LOW_LATENCY) ? 1 : 0;
	tty->alt_speed = 0;
	state->info->tty = tty;

	if (tty_hung_up_p(filp)) {
		retval = -EAGAIN;
		state->count--;
		MP_STATE_UNLOCK(state);
		goto fail;
	}

	if (state->count == 1)
		mp_change_pm(state, 0);

	retval = mp_startup(state, 0);

	if (retval == 0)
		retval = mp_block_til_ready(filp, state);
	MP_STATE_UNLOCK(state);

	if (retval == 0 && !(state->info->flags & UIF_NORMAL_ACTIVE)) {
		state->info->flags |= UIF_NORMAL_ACTIVE;

		mp_update_termios(state);
	}

	uart_clear_mctrl(state->port, TIOCM_RTS);
	try_module_get(THIS_MODULE);
fail:
	return retval;
}