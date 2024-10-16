static int serial_ir_open(struct rc_dev *rcdev)
{
	unsigned long flags;

	/* initialize timestamp */
	serial_ir.lastkt = ktime_get();

	spin_lock_irqsave(&hardware[type].lock, flags);

	/* Set DLAB 0. */
	soutp(UART_LCR, sinp(UART_LCR) & (~UART_LCR_DLAB));

	soutp(UART_IER, sinp(UART_IER) | UART_IER_MSI);

	spin_unlock_irqrestore(&hardware[type].lock, flags);

	return 0;
}