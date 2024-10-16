static int mp_chars_in_buffer(struct tty_struct *tty)
{
	struct sb_uart_state *state = tty->driver_data;

	return uart_circ_chars_pending(&state->info->xmit);
}