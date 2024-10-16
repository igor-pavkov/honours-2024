static int set_deep_fifo(struct sb_uart_port *port, int status)
{
	int afr_status = 0;
	afr_status = sb1054_get_register(port, PAGE_4, SB105X_AFR);

	if(status == ENABLE)
	{
		afr_status |= SB105X_AFR_AFEN;
	}
	else
	{
		afr_status &= ~SB105X_AFR_AFEN;
	}
		
	sb1054_set_register(port,PAGE_4,SB105X_AFR,afr_status);
	sb1054_set_register(port,PAGE_4,SB105X_TTR,ttr[port->line]); 
	sb1054_set_register(port,PAGE_4,SB105X_RTR,rtr[port->line]); 
	afr_status = sb1054_get_register(port, PAGE_4, SB105X_AFR);
		
	return afr_status;
}