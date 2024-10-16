static unsigned int multi_tx_empty(struct sb_uart_port *port)
{
	struct mp_port *mtpt = (struct mp_port *)port;
	unsigned long flags;
	unsigned int ret;

	spin_lock_irqsave(&mtpt->port.lock, flags);
	ret = serial_in(mtpt, UART_LSR) & UART_LSR_TEMT ? TIOCSER_TEMT : 0;
	spin_unlock_irqrestore(&mtpt->port.lock, flags);

	return ret;
}