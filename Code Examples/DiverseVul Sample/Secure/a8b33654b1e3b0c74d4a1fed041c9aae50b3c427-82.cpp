static int get_deep_fifo(struct sb_uart_port *port)
{
	int afr_status = 0;
	afr_status = sb1054_get_register(port, PAGE_4, SB105X_AFR);
	return afr_status;
}