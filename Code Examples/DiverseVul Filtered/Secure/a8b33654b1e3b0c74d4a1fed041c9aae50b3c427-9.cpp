static int sb1054_set_register(struct sb_uart_port *port, int page, int reg, int value)
{  
	int lcr = 0;
	int mcr = 0;
	int ret = 0;

	if( page <= 0)
	{
		printk(" page 0 can not use this fuction\n");
		return -1;
	}
	switch(page)
	{
		case 1:
			lcr = SB105X_GET_LCR(port);
			SB105X_PUT_LCR(port, lcr | SB105X_LCR_DLAB);

			SB105X_PUT_REG(port,reg,value);

			SB105X_PUT_LCR(port, lcr);
			ret = 1;
			break;
		case 2:
			mcr = SB105X_GET_MCR(port);
			SB105X_PUT_MCR(port, mcr | SB105X_MCR_P2S);

			SB105X_PUT_REG(port,reg,value);

			SB105X_PUT_MCR(port, mcr);
			ret = 1;
			break;
		case 3:
			lcr = SB105X_GET_LCR(port);
			SB105X_PUT_LCR(port, lcr | SB105X_LCR_BF);
			SB105X_PUT_PSR(port, SB105X_PSR_P3KEY);

			SB105X_PUT_REG(port,reg,value);

			SB105X_PUT_LCR(port, lcr);
			ret = 1;
			break;
		case 4:
			lcr = SB105X_GET_LCR(port);
			SB105X_PUT_LCR(port, lcr | SB105X_LCR_BF);
			SB105X_PUT_PSR(port, SB105X_PSR_P4KEY);

			SB105X_PUT_REG(port,reg,value);

			SB105X_PUT_LCR(port, lcr);
			ret = 1;
			break;
		default:
			printk(" error invalid page number \n");
			return -1;
	}

	return ret;
}