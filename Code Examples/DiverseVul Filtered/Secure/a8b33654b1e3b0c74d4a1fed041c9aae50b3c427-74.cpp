static int mp_ioctl(struct tty_struct *tty, unsigned int cmd, unsigned long arg)
{
	struct sb_uart_state *state = tty->driver_data;
	struct mp_port *info = (struct mp_port *)state->port;
	int ret = -ENOIOCTLCMD;


	switch (cmd) {
		case TIOCSMULTIDROP:
			/* set multi-drop mode enable or disable, and default operation mode is H/W mode */
			if (info->port.type == PORT_16C105XA)
			{
				//arg &= ~0x6;
				//state->port->mdmode = 0;
				return set_multidrop_mode((struct sb_uart_port *)info, (unsigned int)arg);
			}
			ret = -ENOTSUPP;
			break;
		case GETDEEPFIFO:
			ret = get_deep_fifo(state->port);
			return ret;
		case SETDEEPFIFO:
			ret = set_deep_fifo(state->port,arg);
			deep[state->port->line] = arg;
			return ret;
		case SETTTR:
			if (info->port.type == PORT_16C105X || info->port.type == PORT_16C105XA){
				ret = sb1054_set_register(state->port,PAGE_4,SB105X_TTR,arg);
				ttr[state->port->line] = arg;
			}
			return ret;
		case SETRTR:
			if (info->port.type == PORT_16C105X || info->port.type == PORT_16C105XA){
				ret = sb1054_set_register(state->port,PAGE_4,SB105X_RTR,arg);
				rtr[state->port->line] = arg;
			}
			return ret;
		case GETTTR:
			if (info->port.type == PORT_16C105X || info->port.type == PORT_16C105XA){
				ret = sb1054_get_register(state->port,PAGE_4,SB105X_TTR);
			}
			return ret;
		case GETRTR:
			if (info->port.type == PORT_16C105X || info->port.type == PORT_16C105XA){
				ret = sb1054_get_register(state->port,PAGE_4,SB105X_RTR);
			}
			return ret;

		case SETFCR:
			if (info->port.type == PORT_16C105X || info->port.type == PORT_16C105XA){
				ret = sb1054_set_register(state->port,PAGE_1,SB105X_FCR,arg);
			}
			else{
				serial_out(info,2,arg);
			}

			return ret;
		case TIOCSMDADDR:
			/* set multi-drop address */
			if (info->port.type == PORT_16C105XA)
			{
				state->port->mdmode |= MDMODE_ADDR;
				return set_multidrop_addr((struct sb_uart_port *)info, (unsigned int)arg);
			}
			ret = -ENOTSUPP;
			break;

		case TIOCGMDADDR:
			/* set multi-drop address */
			if ((info->port.type == PORT_16C105XA) && (state->port->mdmode & MDMODE_ADDR))
			{
				return get_multidrop_addr((struct sb_uart_port *)info);
			}
			ret = -ENOTSUPP;
			break;

		case TIOCSENDADDR:
			/* send address in multi-drop mode */
			if ((info->port.type == PORT_16C105XA) 
					&& (state->port->mdmode & (MDMODE_ENABLE)))
			{
				if (mp_chars_in_buffer(tty) > 0)
				{
					tty_wait_until_sent(tty, 0);
				}
				//while ((serial_in(info, UART_LSR) & 0x60) != 0x60);
				//while (sb1054_get_register(state->port, PAGE_2, SB105X_TFCR) != 0);
				while ((serial_in(info, UART_LSR) & 0x60) != 0x60);
				serial_out(info, UART_SCR, (int)arg);
			}
			break;

		case TIOCGSERIAL:
			ret = mp_get_info(state, (struct serial_struct *)arg);
			break;

		case TIOCSSERIAL:
			ret = mp_set_info(state, (struct serial_struct *)arg);
			break;

		case TIOCSERCONFIG:
			ret = mp_do_autoconfig(state);
			break;

		case TIOCSERGWILD: /* obsolete */
		case TIOCSERSWILD: /* obsolete */
			ret = 0;
			break;
			/* for Multiport */
		case TIOCGNUMOFPORT: /* Get number of ports */
			return NR_PORTS;
		case TIOCGGETDEVID:
			return mp_devs[arg].device_id;
		case TIOCGGETREV:
			return mp_devs[arg].revision;
		case TIOCGGETNRPORTS:
			return mp_devs[arg].nr_ports;
		case TIOCGGETBDNO:
			return NR_BOARD;
		case TIOCGGETINTERFACE:
			if (mp_devs[arg].revision == 0xc0)
			{
				/* for SB16C1053APCI */
				return (sb1053a_get_interface(info, info->port.line));
			}
			else
			{
				return (inb(mp_devs[arg].option_reg_addr+MP_OPTR_IIR0+(state->port->line/8)));
			}
		case TIOCGGETPORTTYPE:
			ret = get_device_type(arg);;
			return ret;
		case TIOCSMULTIECHO: /* set to multi-drop mode(RS422) or echo mode(RS485)*/
			outb( ( inb(info->interface_config_addr) & ~0x03 ) | 0x01 ,  
					info->interface_config_addr);
			return 0;
		case TIOCSPTPNOECHO: /* set to multi-drop mode(RS422) or echo mode(RS485) */
			outb( ( inb(info->interface_config_addr) & ~0x03 )  ,             
					info->interface_config_addr);
			return 0;
	}

	if (ret != -ENOIOCTLCMD)
		goto out;

	if (tty->flags & (1 << TTY_IO_ERROR)) {
		ret = -EIO;
		goto out;
	}

	switch (cmd) {
		case TIOCMIWAIT:
			ret = mp_wait_modem_status(state, arg);
			break;

		case TIOCGICOUNT:
			ret = mp_get_count(state, (struct serial_icounter_struct *)arg);
			break;
	}

	if (ret != -ENOIOCTLCMD)
		goto out;

	MP_STATE_LOCK(state);
	switch (cmd) {
		case TIOCSERGETLSR: /* Get line status register */
			ret = mp_get_lsr_info(state, (unsigned int *)arg);
			break;

		default: {
					struct sb_uart_port *port = state->port;
					if (port->ops->ioctl)
						ret = port->ops->ioctl(port, cmd, arg);
					break;
				}
	}

	MP_STATE_UNLOCK(state);
out:
	return ret;
}