static void multi_pm(struct sb_uart_port *port, unsigned int state, unsigned int oldstate)
{
	struct mp_port *mtpt = (struct mp_port *)port;
	if (state) {
		if (mtpt->capabilities & UART_STARTECH) {
			serial_outp(mtpt, UART_LCR, 0xBF);
			serial_outp(mtpt, UART_EFR, UART_EFR_ECB);
			serial_outp(mtpt, UART_LCR, 0);
			serial_outp(mtpt, UART_IER, UART_IERX_SLEEP);
			serial_outp(mtpt, UART_LCR, 0xBF);
			serial_outp(mtpt, UART_EFR, 0);
			serial_outp(mtpt, UART_LCR, 0);
		}

		if (mtpt->pm)
			mtpt->pm(port, state, oldstate);
	} 
	else 
	{
		if (mtpt->capabilities & UART_STARTECH) {
			serial_outp(mtpt, UART_LCR, 0xBF);
			serial_outp(mtpt, UART_EFR, UART_EFR_ECB);
			serial_outp(mtpt, UART_LCR, 0);
			serial_outp(mtpt, UART_IER, 0);
			serial_outp(mtpt, UART_LCR, 0xBF);
			serial_outp(mtpt, UART_EFR, 0);
			serial_outp(mtpt, UART_LCR, 0);
		}

		if (mtpt->pm)
			mtpt->pm(port, state, oldstate);
	}
}