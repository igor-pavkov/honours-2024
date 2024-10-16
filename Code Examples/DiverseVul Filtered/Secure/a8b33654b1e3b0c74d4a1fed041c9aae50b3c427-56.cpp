static void autoconfig_irq(struct mp_port *mtpt)
{
	unsigned char save_mcr, save_ier;
	unsigned long irqs;
	int irq;

	/* forget possible initially masked and pending IRQ */
	probe_irq_off(probe_irq_on());
	save_mcr = serial_inp(mtpt, UART_MCR);
	save_ier = serial_inp(mtpt, UART_IER);
	serial_outp(mtpt, UART_MCR, UART_MCR_OUT1 | UART_MCR_OUT2);

	irqs = probe_irq_on();
	serial_outp(mtpt, UART_MCR, 0);
	serial_outp(mtpt, UART_MCR,
		UART_MCR_DTR | UART_MCR_RTS | UART_MCR_OUT2);

	serial_outp(mtpt, UART_IER, 0x0f);    /* enable all intrs */
	(void)serial_inp(mtpt, UART_LSR);
	(void)serial_inp(mtpt, UART_RX);
	(void)serial_inp(mtpt, UART_IIR);
	(void)serial_inp(mtpt, UART_MSR);
	serial_outp(mtpt, UART_TX, 0xFF);
	irq = probe_irq_off(irqs);

	serial_outp(mtpt, UART_MCR, save_mcr);
	serial_outp(mtpt, UART_IER, save_ier);

	mtpt->port.irq = (irq > 0) ? irq : 0;
}