static int set_multidrop_addr(struct sb_uart_port *port, unsigned int addr)
{
	sb1054_set_register(port, PAGE_3, SB105X_XOFF2, addr);

	return 0;
}