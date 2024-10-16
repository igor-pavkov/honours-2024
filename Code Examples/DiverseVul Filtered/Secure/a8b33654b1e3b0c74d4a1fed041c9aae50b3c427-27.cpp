static int mp_get_lsr_info(struct sb_uart_state *state, unsigned int *value)
{
	struct sb_uart_port *port = state->port;
	unsigned int result;

	result = port->ops->tx_empty(port);

	if (port->x_char ||
			((uart_circ_chars_pending(&state->info->xmit) > 0) &&
				!state->info->tty->stopped && !state->info->tty->hw_stopped))
		result &= ~TIOCSER_TEMT;

	return put_user(result, value);
}