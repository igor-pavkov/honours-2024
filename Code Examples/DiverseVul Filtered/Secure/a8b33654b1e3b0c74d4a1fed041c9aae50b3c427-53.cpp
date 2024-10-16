static void autoconfig(struct mp_port *mtpt, unsigned int probeflags)
{
	unsigned char status1, scratch, scratch2, scratch3;
	unsigned char save_lcr, save_mcr;
	unsigned long flags;

	unsigned char u_type;
	unsigned char b_ret = 0;

	if (!mtpt->port.iobase && !mtpt->port.mapbase && !mtpt->port.membase)
		return;

	DEBUG_AUTOCONF("ttyMP%d: autoconf (0x%04x, 0x%p): ",
			mtpt->port.line, mtpt->port.iobase, mtpt->port.membase);

	spin_lock_irqsave(&mtpt->port.lock, flags);

	if (!(mtpt->port.flags & UPF_BUGGY_UART)) {
		scratch = serial_inp(mtpt, UART_IER);
		serial_outp(mtpt, UART_IER, 0);
#ifdef __i386__
		outb(0xff, 0x080);
#endif
		scratch2 = serial_inp(mtpt, UART_IER) & 0x0f;
		serial_outp(mtpt, UART_IER, 0x0F);
#ifdef __i386__
		outb(0, 0x080);
#endif
		scratch3 = serial_inp(mtpt, UART_IER) & 0x0F;
		serial_outp(mtpt, UART_IER, scratch);
		if (scratch2 != 0 || scratch3 != 0x0F) {
			DEBUG_AUTOCONF("IER test failed (%02x, %02x) ",
					scratch2, scratch3);
			goto out;
		}
	}

	save_mcr = serial_in(mtpt, UART_MCR);
	save_lcr = serial_in(mtpt, UART_LCR);

	if (!(mtpt->port.flags & UPF_SKIP_TEST)) {
		serial_outp(mtpt, UART_MCR, UART_MCR_LOOP | 0x0A);
		status1 = serial_inp(mtpt, UART_MSR) & 0xF0;
		serial_outp(mtpt, UART_MCR, save_mcr);
		if (status1 != 0x90) {
			DEBUG_AUTOCONF("LOOP test failed (%02x) ",
					status1);
			goto out;
		}
	}

	serial_outp(mtpt, UART_LCR, 0xBF);
	serial_outp(mtpt, UART_EFR, 0);
	serial_outp(mtpt, UART_LCR, 0);

	serial_outp(mtpt, UART_FCR, UART_FCR_ENABLE_FIFO);
	scratch = serial_in(mtpt, UART_IIR) >> 6;

	DEBUG_AUTOCONF("iir=%d ", scratch);
	if(mtpt->device->nr_ports >= 8)
		b_ret = read_option_register(mtpt,(MP_OPTR_DIR0 + ((mtpt->port.line)/8)));
	else	
		b_ret = read_option_register(mtpt,MP_OPTR_DIR0);
	u_type = (b_ret & 0xf0) >> 4;
	if(mtpt->port.type == PORT_UNKNOWN )
	{
		switch (u_type)
		{
			case DIR_UART_16C550:
				mtpt->port.type = PORT_16C55X;
				break;
			case DIR_UART_16C1050:
				mtpt->port.type = PORT_16C105X;
				break;
			case DIR_UART_16C1050A:
				if (mtpt->port.line < 2)
				{
					mtpt->port.type = PORT_16C105XA;
				}
				else
				{
					if (mtpt->device->device_id & 0x50)
					{
						mtpt->port.type = PORT_16C55X;
					}
					else
					{
						mtpt->port.type = PORT_16C105X;
					}
				}
				break;
			default:	
				mtpt->port.type = PORT_UNKNOWN;
				break;
		}
	}

	if(mtpt->port.type == PORT_UNKNOWN )
	{
printk("unknow2\n");
		switch (scratch) {
			case 0:
			case 1:
				mtpt->port.type = PORT_UNKNOWN;
				break;
			case 2:
			case 3:
				mtpt->port.type = PORT_16C55X;
				break;
		}
	}

	serial_outp(mtpt, UART_LCR, save_lcr);

	mtpt->port.fifosize = uart_config[mtpt->port.type].dfl_xmit_fifo_size;
	mtpt->capabilities = uart_config[mtpt->port.type].flags;

	if (mtpt->port.type == PORT_UNKNOWN)
		goto out;
	serial_outp(mtpt, UART_MCR, save_mcr);
	serial_outp(mtpt, UART_FCR, (UART_FCR_ENABLE_FIFO |
				UART_FCR_CLEAR_RCVR |
				UART_FCR_CLEAR_XMIT));
	serial_outp(mtpt, UART_FCR, 0);
	(void)serial_in(mtpt, UART_RX);
	serial_outp(mtpt, UART_IER, 0);

out:
	spin_unlock_irqrestore(&mtpt->port.lock, flags);
	DEBUG_AUTOCONF("type=%s\n", uart_config[mtpt->port.type].name);
}