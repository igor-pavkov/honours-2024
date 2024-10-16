static void multi_set_termios(struct sb_uart_port *port, struct MP_TERMIOS *termios, struct MP_TERMIOS *old)
{
	struct mp_port *mtpt = (struct mp_port *)port;
	unsigned char cval, fcr = 0;
	unsigned long flags;
	unsigned int baud, quot;

	switch (termios->c_cflag & CSIZE) {
		case CS5:
			cval = 0x00;
			break;
		case CS6:
			cval = 0x01;
			break;
		case CS7:
			cval = 0x02;
			break;
		default:
		case CS8:
			cval = 0x03;
			break;
	}

	if (termios->c_cflag & CSTOPB)
		cval |= 0x04;
	if (termios->c_cflag & PARENB)
		cval |= UART_LCR_PARITY;
	if (!(termios->c_cflag & PARODD))
		cval |= UART_LCR_EPAR;

#ifdef CMSPAR
	if (termios->c_cflag & CMSPAR)
		cval |= UART_LCR_SPAR;
#endif

	baud = sb_uart_get_baud_rate(port, termios, old, 0, port->uartclk/16);
	quot = multi_get_divisor(port, baud);

	if (mtpt->capabilities & UART_USE_FIFO) {
		//if (baud < 2400)
		//	fcr = UART_FCR_ENABLE_FIFO | UART_FCR_TRIGGER_1;
		//else
		//	fcr = UART_FCR_ENABLE_FIFO | UART_FCR_TRIGGER_8;

		//	fcr = UART_FCR_ENABLE_FIFO | 0x90;
			fcr = fcr_arr[mtpt->port.line];
	}

	spin_lock_irqsave(&mtpt->port.lock, flags);

	sb_uart_update_timeout(port, termios->c_cflag, baud);

	mtpt->port.read_status_mask = UART_LSR_OE | UART_LSR_THRE | UART_LSR_DR;
	if (termios->c_iflag & INPCK)
		mtpt->port.read_status_mask |= UART_LSR_FE | UART_LSR_PE;
	if (termios->c_iflag & (BRKINT | PARMRK))
		mtpt->port.read_status_mask |= UART_LSR_BI;

	mtpt->port.ignore_status_mask = 0;
	if (termios->c_iflag & IGNPAR)
		mtpt->port.ignore_status_mask |= UART_LSR_PE | UART_LSR_FE;
	if (termios->c_iflag & IGNBRK) {
		mtpt->port.ignore_status_mask |= UART_LSR_BI;
		if (termios->c_iflag & IGNPAR)
			mtpt->port.ignore_status_mask |= UART_LSR_OE;
	}

	if ((termios->c_cflag & CREAD) == 0)
		mtpt->port.ignore_status_mask |= UART_LSR_DR;

	mtpt->ier &= ~UART_IER_MSI;
	if (UART_ENABLE_MS(&mtpt->port, termios->c_cflag))
		mtpt->ier |= UART_IER_MSI;

	serial_out(mtpt, UART_IER, mtpt->ier);

	if (mtpt->capabilities & UART_STARTECH) {
		serial_outp(mtpt, UART_LCR, 0xBF);
		serial_outp(mtpt, UART_EFR,
				termios->c_cflag & CRTSCTS ? UART_EFR_CTS :0);
	}

	serial_outp(mtpt, UART_LCR, cval | UART_LCR_DLAB);/* set DLAB */

	serial_outp(mtpt, UART_DLL, quot & 0xff);     /* LS of divisor */
	serial_outp(mtpt, UART_DLM, quot >> 8);       /* MS of divisor */

	serial_outp(mtpt, UART_LCR, cval);        /* reset DLAB */
	mtpt->lcr = cval;                 /* Save LCR */

	if (fcr & UART_FCR_ENABLE_FIFO) {
		/* emulated UARTs (Lucent Venus 167x) need two steps */
		serial_outp(mtpt, UART_FCR, UART_FCR_ENABLE_FIFO);
	}

	serial_outp(mtpt, UART_FCR, fcr);     /* set fcr */


	if ((mtpt->port.type == PORT_16C105X)
		|| (mtpt->port.type == PORT_16C105XA))
	{
		if(deep[mtpt->port.line]!=0)
			set_deep_fifo(port, ENABLE);

		if (mtpt->interface != RS232)
			set_auto_rts(port,mtpt->interface);

	}
	else
	{
		if (mtpt->interface >= RS485NE)
		{
			uart_clear_mctrl(&mtpt->port, TIOCM_RTS);
		}
	}

	if(mtpt->device->device_id == PCI_DEVICE_ID_MP4M)
	{
		SendATCommand(mtpt);
		printk("SendATCommand\n");
	}	
	multi_set_mctrl(&mtpt->port, mtpt->port.mctrl);
	spin_unlock_irqrestore(&mtpt->port.lock, flags);
}