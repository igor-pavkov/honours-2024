static int mp_get_info(struct sb_uart_state *state, struct serial_struct *retinfo)
{
	struct sb_uart_port *port = state->port;
	struct serial_struct tmp;

	memset(&tmp, 0, sizeof(tmp));
	tmp.type	    = port->type;
	tmp.line	    = port->line;
	tmp.port	    = port->iobase;
	if (HIGH_BITS_OFFSET)
		tmp.port_high = (long) port->iobase >> HIGH_BITS_OFFSET;
	tmp.irq		    = port->irq;
	tmp.flags	    = port->flags;
	tmp.xmit_fifo_size  = port->fifosize;
	tmp.baud_base	    = port->uartclk / 16;
	tmp.close_delay	    = state->close_delay;
	tmp.closing_wait    = state->closing_wait == USF_CLOSING_WAIT_NONE ?
		ASYNC_CLOSING_WAIT_NONE :
		state->closing_wait;
	tmp.custom_divisor  = port->custom_divisor;
	tmp.hub6	    = port->hub6;
	tmp.io_type         = port->iotype;
	tmp.iomem_reg_shift = port->regshift;
	tmp.iomem_base      = (void *)port->mapbase;

	if (copy_to_user(retinfo, &tmp, sizeof(*retinfo)))
		return -EFAULT;
	return 0;
}