static unsigned int multi_get_divisor(struct sb_uart_port *port, unsigned int baud)
{
	unsigned int quot;

	if ((port->flags & UPF_MAGIC_MULTIPLIER) &&
			baud == (port->uartclk/4))
		quot = 0x8001;
	else if ((port->flags & UPF_MAGIC_MULTIPLIER) &&
			baud == (port->uartclk/8))
		quot = 0x8002;
	else
		quot = sb_uart_get_divisor(port, baud);

	return quot;
}