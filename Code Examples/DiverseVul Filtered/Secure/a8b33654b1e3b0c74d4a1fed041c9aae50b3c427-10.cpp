static void multi_shutdown(struct sb_uart_port *port)
{
	struct mp_port *mtpt = (struct mp_port *)port;
	unsigned long flags;


	mtpt->ier = 0;
	serial_outp(mtpt, UART_IER, 0);

	spin_lock_irqsave(&mtpt->port.lock, flags);
	mtpt->port.mctrl &= ~TIOCM_OUT2;

	multi_set_mctrl(&mtpt->port, mtpt->port.mctrl);
	spin_unlock_irqrestore(&mtpt->port.lock, flags);

	serial_out(mtpt, UART_LCR, serial_inp(mtpt, UART_LCR) & ~UART_LCR_SBC);
	serial_outp(mtpt, UART_FCR, UART_FCR_ENABLE_FIFO |
			UART_FCR_CLEAR_RCVR |
			UART_FCR_CLEAR_XMIT);
	serial_outp(mtpt, UART_FCR, 0);


	(void) serial_in(mtpt, UART_RX);

	if ((!is_real_interrupt(mtpt->port.irq))||(mtpt->poll_type==TYPE_POLL))
	{
		del_timer_sync(&mtpt->timer);
	}
	else
	{
		serial_unlink_irq_chain(mtpt);
	}
}