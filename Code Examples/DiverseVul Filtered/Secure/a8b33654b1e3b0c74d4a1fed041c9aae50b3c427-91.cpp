static void mp_shutdown(struct sb_uart_state *state)
{
	struct sb_uart_info *info = state->info;
	struct sb_uart_port *port = state->port;

	if (info->tty)
		set_bit(TTY_IO_ERROR, &info->tty->flags);

	if (info->flags & UIF_INITIALIZED) {
		info->flags &= ~UIF_INITIALIZED;

		if (!info->tty || (info->tty->termios.c_cflag & HUPCL))
			uart_clear_mctrl(port, TIOCM_DTR | TIOCM_RTS);

		wake_up_interruptible(&info->delta_msr_wait);

		port->ops->shutdown(port);

		synchronize_irq(port->irq);
	}
	tasklet_kill(&info->tlet);

	if (info->xmit.buf) {
		free_page((unsigned long)info->xmit.buf);
		info->xmit.buf = NULL;
	}
}