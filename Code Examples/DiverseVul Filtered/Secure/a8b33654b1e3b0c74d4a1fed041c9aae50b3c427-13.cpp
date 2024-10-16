static void multi_stop_tx(struct sb_uart_port *port)
{
	struct mp_port *mtpt = (struct mp_port *)port;

	if (mtpt->ier & UART_IER_THRI) {
		mtpt->ier &= ~UART_IER_THRI;
		serial_out(mtpt, UART_IER, mtpt->ier);
	}

	tasklet_schedule(&port->info->tlet);
}