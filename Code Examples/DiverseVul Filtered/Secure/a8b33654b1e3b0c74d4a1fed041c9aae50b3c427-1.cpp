static int get_multidrop_addr(struct sb_uart_port *port)
{
	return sb1054_get_register(port, PAGE_3, SB105X_XOFF2);
}