static int mp_write_room(struct tty_struct *tty)
{
	struct sb_uart_state *state = tty->driver_data;

	return uart_circ_chars_free(&state->info->xmit);
}