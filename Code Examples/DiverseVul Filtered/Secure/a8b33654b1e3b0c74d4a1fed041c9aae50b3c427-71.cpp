static int mp_put_char(struct tty_struct *tty, unsigned char ch)
{
	struct sb_uart_state *state = tty->driver_data;

	return __mp_put_char(state->port, &state->info->xmit, ch);
}