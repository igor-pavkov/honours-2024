static unsigned int multi_get_mctrl(struct sb_uart_port *port)
{
	struct mp_port *mtpt = (struct mp_port *)port;
	unsigned char status;
	unsigned int ret;

	status = serial_in(mtpt, UART_MSR);

	ret = 0;
	if (status & UART_MSR_DCD)
		ret |= TIOCM_CAR;
	if (status & UART_MSR_RI)
		ret |= TIOCM_RNG;
	if (status & UART_MSR_DSR)
		ret |= TIOCM_DSR;
	if (status & UART_MSR_CTS)
		ret |= TIOCM_CTS;
	return ret;
}