static int serial_ir_resume(struct platform_device *dev)
{
	unsigned long flags;
	int result;

	result = hardware_init_port();
	if (result < 0)
		return result;

	spin_lock_irqsave(&hardware[type].lock, flags);
	/* Enable Interrupt */
	serial_ir.lastkt = ktime_get();
	soutp(UART_IER, sinp(UART_IER) | UART_IER_MSI);
	off();

	spin_unlock_irqrestore(&hardware[type].lock, flags);

	return 0;
}