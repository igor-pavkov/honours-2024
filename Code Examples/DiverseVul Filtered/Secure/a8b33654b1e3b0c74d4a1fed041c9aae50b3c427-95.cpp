static int set_multidrop_mode(struct sb_uart_port *port, unsigned int mode)
{
	int mdr = SB105XA_MDR_NPS;

	if (mode & MDMODE_ENABLE)
	{
		mdr |= SB105XA_MDR_MDE;
	}

	if (1) //(mode & MDMODE_AUTO)
	{
		int efr = 0;
		mdr |= SB105XA_MDR_AME;
		efr = sb1054_get_register(port, PAGE_3, SB105X_EFR);
		efr |= SB105X_EFR_SCD;
		sb1054_set_register(port, PAGE_3, SB105X_EFR, efr);
	}

	sb1054_set_register(port, PAGE_1, SB105XA_MDR, mdr);
	port->mdmode &= ~0x6;
	port->mdmode |= mode;
	printk("[%d] multidrop init: %x\n", port->line, port->mdmode);

	return 0;
}