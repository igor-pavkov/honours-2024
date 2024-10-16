static int mp_write(struct tty_struct *tty, const unsigned char *buf, int count)
{
	struct sb_uart_state *state = tty->driver_data;
	struct sb_uart_port *port;
	struct circ_buf *circ;
	int c, ret = 0;

	if (!state || !state->info) {
		return -EL3HLT;
	}

	port = state->port;
	circ = &state->info->xmit;

	if (!circ->buf)
		return 0;
		
	while (1) {
		c = CIRC_SPACE_TO_END(circ->head, circ->tail, UART_XMIT_SIZE);
		if (count < c)
			c = count;
		if (c <= 0)
			break;
	memcpy(circ->buf + circ->head, buf, c);

		circ->head = (circ->head + c) & (UART_XMIT_SIZE - 1);
		buf += c;
		count -= c;
		ret += c;
	}
	mp_start(tty);
	return ret;
}