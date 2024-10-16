static inline void multi_handle_port(struct mp_port *mtpt)
{
	unsigned int status = serial_inp(mtpt, UART_LSR);

	//printk("lsr: %x\n", status);

	if ((status & UART_LSR_DR) || (status & UART_LSR_SPECIAL))
		receive_chars(mtpt, &status);
	check_modem_status(mtpt);
	if (status & UART_LSR_THRE)
	{
		if ((mtpt->port.type == PORT_16C105X)
			|| (mtpt->port.type == PORT_16C105XA))
			transmit_chars(mtpt);
		else
		{
			if (mtpt->interface >= RS485NE)
				uart_set_mctrl(&mtpt->port, TIOCM_RTS);
			
			transmit_chars(mtpt);


			if (mtpt->interface >= RS485NE)
			{
				while((status=serial_in(mtpt,UART_LSR) &0x60)!=0x60);
				uart_clear_mctrl(&mtpt->port, TIOCM_RTS);
			}
		}
	}
}