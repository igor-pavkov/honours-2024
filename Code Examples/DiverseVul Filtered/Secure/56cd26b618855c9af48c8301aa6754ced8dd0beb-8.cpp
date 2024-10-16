static void off(void)
{
	if (txsense)
		soutp(UART_MCR, hardware[type].on);
	else
		soutp(UART_MCR, hardware[type].off);
}