static void on(void)
{
	if (txsense)
		soutp(UART_MCR, hardware[type].off);
	else
		soutp(UART_MCR, hardware[type].on);
}