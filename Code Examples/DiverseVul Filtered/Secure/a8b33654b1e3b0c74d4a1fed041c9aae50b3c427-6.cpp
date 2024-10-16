static void multi_start_tx(struct sb_uart_port *port)
{
	struct mp_port *mtpt = (struct mp_port *)port;

	if (!(mtpt->ier & UART_IER_THRI)) {
		mtpt->ier |= UART_IER_THRI;
		serial_out(mtpt, UART_IER, mtpt->ier);
	}
}