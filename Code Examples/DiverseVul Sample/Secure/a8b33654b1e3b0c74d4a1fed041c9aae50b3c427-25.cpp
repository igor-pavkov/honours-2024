static _INLINE_ void receive_chars(struct mp_port *mtpt, int *status )
{
	struct tty_struct *tty = mtpt->port.info->tty;
	unsigned char lsr = *status;
	int max_count = 256;
	unsigned char ch;
	char flag;

	//lsr &= mtpt->port.read_status_mask;

	do {
		if ((lsr & UART_LSR_PE) && (mtpt->port.mdmode & MDMODE_ENABLE))
		{
			ch = serial_inp(mtpt, UART_RX);
		}
		else if (lsr & UART_LSR_SPECIAL) 
		{
			flag = 0;
			ch = serial_inp(mtpt, UART_RX);

			if (lsr & UART_LSR_BI) 
			{

				mtpt->port.icount.brk++;
				flag = TTY_BREAK;

				if (sb_uart_handle_break(&mtpt->port))
					goto ignore_char;
			} 
			if (lsr & UART_LSR_PE)
			{
				mtpt->port.icount.parity++;
				flag = TTY_PARITY;
			}
			if (lsr & UART_LSR_FE)
			{
				mtpt->port.icount.frame++;
				flag = TTY_FRAME;
			}
			if (lsr & UART_LSR_OE)
			{
				mtpt->port.icount.overrun++;
				flag = TTY_OVERRUN;
			}
			tty_insert_flip_char(tty, ch, flag);
		}
		else
		{
			ch = serial_inp(mtpt, UART_RX);
			tty_insert_flip_char(tty, ch, 0);
		}
ignore_char:
		lsr = serial_inp(mtpt, UART_LSR);
	} while ((lsr & UART_LSR_DR) && (max_count-- > 0));

	tty_flip_buffer_push(tty);
}