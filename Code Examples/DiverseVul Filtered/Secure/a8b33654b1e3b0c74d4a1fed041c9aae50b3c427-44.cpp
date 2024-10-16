static int sb1054_get_register(struct sb_uart_port *port, int page, int reg)
{
	int ret = 0;
	unsigned int lcr = 0;
	unsigned int mcr = 0;
	unsigned int tmp = 0;

	if( page <= 0)
	{
		printk(" page 0 can not use this fuction\n");
		return -1;
	}

	switch(page)
	{
		case 1:
			lcr = SB105X_GET_LCR(port);
			tmp = lcr | SB105X_LCR_DLAB;
			SB105X_PUT_LCR(port, tmp);

			tmp = SB105X_GET_LCR(port);

			ret = SB105X_GET_REG(port,reg);
			SB105X_PUT_LCR(port,lcr);
			break;
		case 2:
			mcr = SB105X_GET_MCR(port);
			tmp = mcr | SB105X_MCR_P2S;
			SB105X_PUT_MCR(port,tmp);

			ret = SB105X_GET_REG(port,reg);

			SB105X_PUT_MCR(port,mcr);
			break;
		case 3:
			lcr = SB105X_GET_LCR(port);
			tmp = lcr | SB105X_LCR_BF;
			SB105X_PUT_LCR(port,tmp);
			SB105X_PUT_REG(port,SB105X_PSR,SB105X_PSR_P3KEY);

			ret = SB105X_GET_REG(port,reg);

			SB105X_PUT_LCR(port,lcr);
			break;
		case 4:
			lcr = SB105X_GET_LCR(port);
			tmp = lcr | SB105X_LCR_BF;
			SB105X_PUT_LCR(port,tmp);
			SB105X_PUT_REG(port,SB105X_PSR,SB105X_PSR_P4KEY);

			ret = SB105X_GET_REG(port,reg);

			SB105X_PUT_LCR(port,lcr);
			break;
		default:
			printk(" error invalid page number \n");
			return -1;
	}

	return ret;
}