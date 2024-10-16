static int hardware_init_port(void)
{
	u8 scratch, scratch2, scratch3;

	/*
	 * This is a simple port existence test, borrowed from the autoconfig
	 * function in drivers/tty/serial/8250/8250_port.c
	 */
	scratch = sinp(UART_IER);
	soutp(UART_IER, 0);
#ifdef __i386__
	outb(0xff, 0x080);
#endif
	scratch2 = sinp(UART_IER) & 0x0f;
	soutp(UART_IER, 0x0f);
#ifdef __i386__
	outb(0x00, 0x080);
#endif
	scratch3 = sinp(UART_IER) & 0x0f;
	soutp(UART_IER, scratch);
	if (scratch2 != 0 || scratch3 != 0x0f) {
		/* we fail, there's nothing here */
		pr_err("port existence test failed, cannot continue\n");
		return -ENODEV;
	}

	/* Set DLAB 0. */
	soutp(UART_LCR, sinp(UART_LCR) & (~UART_LCR_DLAB));

	/* First of all, disable all interrupts */
	soutp(UART_IER, sinp(UART_IER) &
	      (~(UART_IER_MSI | UART_IER_RLSI | UART_IER_THRI | UART_IER_RDI)));

	/* Clear registers. */
	sinp(UART_LSR);
	sinp(UART_RX);
	sinp(UART_IIR);
	sinp(UART_MSR);

	/* Set line for power source */
	off();

	/* Clear registers again to be sure. */
	sinp(UART_LSR);
	sinp(UART_RX);
	sinp(UART_IIR);
	sinp(UART_MSR);

	switch (type) {
	case IR_IRDEO:
	case IR_IRDEO_REMOTE:
		/* setup port to 7N1 @ 115200 Baud */
		/* 7N1+start = 9 bits at 115200 ~ 3 bits at 38kHz */

		/* Set DLAB 1. */
		soutp(UART_LCR, sinp(UART_LCR) | UART_LCR_DLAB);
		/* Set divisor to 1 => 115200 Baud */
		soutp(UART_DLM, 0);
		soutp(UART_DLL, 1);
		/* Set DLAB 0 +  7N1 */
		soutp(UART_LCR, UART_LCR_WLEN7);
		/* THR interrupt already disabled at this point */
		break;
	default:
		break;
	}

	return 0;
}