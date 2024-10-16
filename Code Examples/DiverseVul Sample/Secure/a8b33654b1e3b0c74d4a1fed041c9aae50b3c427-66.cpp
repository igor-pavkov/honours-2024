static void multi_set_mctrl(struct sb_uart_port *port, unsigned int mctrl)
{
	struct mp_port *mtpt = (struct mp_port *)port;
	unsigned char mcr = 0;

	mctrl &= 0xff;

	if (mctrl & TIOCM_RTS)
		mcr |= UART_MCR_RTS;
	if (mctrl & TIOCM_DTR)
		mcr |= UART_MCR_DTR;
	if (mctrl & TIOCM_OUT1)
		mcr |= UART_MCR_OUT1;
	if (mctrl & TIOCM_OUT2)
		mcr |= UART_MCR_OUT2;
	if (mctrl & TIOCM_LOOP)
		mcr |= UART_MCR_LOOP;


	serial_out(mtpt, UART_MCR, mcr);
}