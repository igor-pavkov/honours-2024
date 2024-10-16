static void multi_stop_rx(struct sb_uart_port *port)
{
	struct mp_port *mtpt = (struct mp_port *)port;

	mtpt->ier &= ~UART_IER_RLSI;
	mtpt->port.read_status_mask &= ~UART_LSR_DR;
	serial_out(mtpt, UART_IER, mtpt->ier);
}