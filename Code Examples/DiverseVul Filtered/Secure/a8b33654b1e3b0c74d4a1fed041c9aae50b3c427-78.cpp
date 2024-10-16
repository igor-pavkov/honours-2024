static _INLINE_ void transmit_chars(struct mp_port *mtpt)
{
	struct circ_buf *xmit = &mtpt->port.info->xmit;
	int count;

	if (mtpt->port.x_char) {
		serial_outp(mtpt, UART_TX, mtpt->port.x_char);
		mtpt->port.icount.tx++;
		mtpt->port.x_char = 0;
		return;
	}
	if (uart_circ_empty(xmit) || uart_tx_stopped(&mtpt->port)) {
		multi_stop_tx(&mtpt->port);
		return;
	}

	count = uart_circ_chars_pending(xmit);

	if(count > mtpt->port.fifosize)
	{
		count = mtpt->port.fifosize;
	}

	printk("[%d] mdmode: %x\n", mtpt->port.line, mtpt->port.mdmode);
	do {
#if 0
		/* check multi-drop mode */
		if ((mtpt->port.mdmode & (MDMODE_ENABLE | MDMODE_ADDR)) == (MDMODE_ENABLE | MDMODE_ADDR))
		{
			printk("send address\n");
			/* send multi-drop address */
			serial_out(mtpt, UART_SCR, xmit->buf[xmit->tail]);
		}
		else
#endif
		{
			serial_out(mtpt, UART_TX, xmit->buf[xmit->tail]);
		}
		xmit->tail = (xmit->tail + 1) & (UART_XMIT_SIZE - 1);
		mtpt->port.icount.tx++;
	} while (--count > 0);
}