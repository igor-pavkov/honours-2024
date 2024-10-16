static void serial_ir_close(struct rc_dev *rcdev)
{
	unsigned long flags;

	spin_lock_irqsave(&hardware[type].lock, flags);

	/* Set DLAB 0. */
	soutp(UART_LCR, sinp(UART_LCR) & (~UART_LCR_DLAB));

	/* First of all, disable all interrupts */
	soutp(UART_IER, sinp(UART_IER) &
	      (~(UART_IER_MSI | UART_IER_RLSI | UART_IER_THRI | UART_IER_RDI)));
	spin_unlock_irqrestore(&hardware[type].lock, flags);
}