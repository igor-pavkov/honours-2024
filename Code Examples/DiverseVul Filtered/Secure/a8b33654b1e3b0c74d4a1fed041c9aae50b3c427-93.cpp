static void mp_close(struct tty_struct *tty, struct file *filp)
{
	struct sb_uart_state *state = tty->driver_data;
	struct sb_uart_port *port;

	printk("mp_close!\n");
	if (!state || !state->port)
		return;

	port = state->port;

	printk("close1 %d\n", __LINE__);
	MP_STATE_LOCK(state);

	printk("close2 %d\n", __LINE__);
	if (tty_hung_up_p(filp))
		goto done;

	printk("close3 %d\n", __LINE__);
	if ((tty->count == 1) && (state->count != 1)) {
		printk("mp_close: bad serial port count; tty->count is 1, "
				"state->count is %d\n", state->count);
		state->count = 1;
	}
	printk("close4 %d\n", __LINE__);
	if (--state->count < 0) {
		printk("rs_close: bad serial port count for ttyMP%d: %d\n",
				port->line, state->count);
		state->count = 0;
	}
	if (state->count)
		goto done;

	tty->closing = 1;

	printk("close5 %d\n", __LINE__);
	if (state->closing_wait != USF_CLOSING_WAIT_NONE)
		tty_wait_until_sent(tty, state->closing_wait);

	printk("close6 %d\n", __LINE__);
	if (state->info->flags & UIF_INITIALIZED) {
		unsigned long flags;
		spin_lock_irqsave(&port->lock, flags);
		port->ops->stop_rx(port);
		spin_unlock_irqrestore(&port->lock, flags);
		mp_wait_until_sent(tty, port->timeout);
	}
	printk("close7 %d\n", __LINE__);

	mp_shutdown(state);
	printk("close8 %d\n", __LINE__);
	mp_flush_buffer(tty);
	tty_ldisc_flush(tty);
	tty->closing = 0;
	state->info->tty = NULL;
	if (state->info->blocked_open) 
	{
		if (state->close_delay)
		{
			set_current_state(TASK_INTERRUPTIBLE);
			schedule_timeout(state->close_delay);
		}
	}
	else
	{
		mp_change_pm(state, 3);
	}
	printk("close8 %d\n", __LINE__);

	state->info->flags &= ~UIF_NORMAL_ACTIVE;
	wake_up_interruptible(&state->info->open_wait);

done:
	printk("close done\n");
	MP_STATE_UNLOCK(state);
	module_put(THIS_MODULE);
}