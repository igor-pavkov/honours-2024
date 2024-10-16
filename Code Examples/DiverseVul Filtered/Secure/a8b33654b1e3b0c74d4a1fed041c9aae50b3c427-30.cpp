static void __init multi_init_ports(void)
{
	struct mp_port *mtpt;
	static int first = 1;
	int i,j,k;
	unsigned char osc;
	unsigned char b_ret = 0;
	static struct mp_device_t *sbdev; 

	if (!first)
		return;
	first = 0;

	mtpt = multi_ports; 

	for (k=0;k<NR_BOARD;k++)
	{
		sbdev = &mp_devs[k];

		for (i = 0; i < sbdev->nr_ports; i++, mtpt++) 
		{
			mtpt->device 		= sbdev;
			mtpt->port.iobase   = sbdev->uart_access_addr + 8*i;
			mtpt->port.irq      = sbdev->irq;
			if ( ((sbdev->device_id == PCI_DEVICE_ID_MP4)&&(sbdev->revision==0x91)))
				mtpt->interface_config_addr = sbdev->option_reg_addr + 0x08 + i;
			else if (sbdev->revision == 0xc0)
				mtpt->interface_config_addr = sbdev->option_reg_addr + 0x08 + (i & 0x1);
			else
				mtpt->interface_config_addr = sbdev->option_reg_addr + 0x08 + i/8;

			mtpt->option_base_addr = sbdev->option_reg_addr;

			mtpt->poll_type = sbdev->poll_type;

			mtpt->port.uartclk  = BASE_BAUD * 16;

			/* get input clock information */
			osc = inb(sbdev->option_reg_addr + MP_OPTR_DIR0 + i/8) & 0x0F;
			if (osc==0x0f)
				osc = 0;
			for(j=0;j<osc;j++)
				mtpt->port.uartclk *= 2;
			mtpt->port.flags    |= STD_COM_FLAGS | UPF_SHARE_IRQ ;
			mtpt->port.iotype   = UPIO_PORT;
			mtpt->port.ops      = &multi_pops;

			if (sbdev->revision == 0xc0)
			{
				/* for SB16C1053APCI */
				b_ret = sb1053a_get_interface(mtpt, i);
			}
			else
			{
				b_ret = read_option_register(mtpt,(MP_OPTR_IIR0 + i/8));
				printk("IIR_RET = %x\n",b_ret);
			}

			/* default to RS232 */
			mtpt->interface = RS232;
			if (IIR_RS422 == (b_ret & IIR_TYPE_MASK))
				mtpt->interface = RS422PTP;
			if (IIR_RS485 == (b_ret & IIR_TYPE_MASK))
				mtpt->interface = RS485NE;
		}
	}
}