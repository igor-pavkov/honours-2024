static void multi_break_ctl(struct sb_uart_port *port, int break_state)
{
	struct mp_port *mtpt = (struct mp_port *)port;
	unsigned long flags;

	spin_lock_irqsave(&mtpt->port.lock, flags);
	if (break_state == -1)
		mtpt->lcr |= UART_LCR_SBC;
	else
		mtpt->lcr &= ~UART_LCR_SBC;
	serial_out(mtpt, UART_LCR, mtpt->lcr);
	spin_unlock_irqrestore(&mtpt->port.lock, flags);
}