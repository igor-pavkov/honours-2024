static int digi_read_oob_callback(struct urb *urb)
{

	struct usb_serial_port *port = urb->context;
	struct usb_serial *serial = port->serial;
	struct tty_struct *tty;
	struct digi_port *priv = usb_get_serial_port_data(port);
	int opcode, line, status, val;
	int i;
	unsigned int rts;

	/* handle each oob command */
	for (i = 0; i < urb->actual_length - 3;) {
		opcode = ((unsigned char *)urb->transfer_buffer)[i++];
		line = ((unsigned char *)urb->transfer_buffer)[i++];
		status = ((unsigned char *)urb->transfer_buffer)[i++];
		val = ((unsigned char *)urb->transfer_buffer)[i++];

		dev_dbg(&port->dev, "digi_read_oob_callback: opcode=%d, line=%d, status=%d, val=%d\n",
			opcode, line, status, val);

		if (status != 0 || line >= serial->type->num_ports)
			continue;

		port = serial->port[line];

		priv = usb_get_serial_port_data(port);
		if (priv == NULL)
			return -1;

		tty = tty_port_tty_get(&port->port);

		rts = 0;
		if (tty)
			rts = C_CRTSCTS(tty);

		if (tty && opcode == DIGI_CMD_READ_INPUT_SIGNALS) {
			spin_lock(&priv->dp_port_lock);
			/* convert from digi flags to termiox flags */
			if (val & DIGI_READ_INPUT_SIGNALS_CTS) {
				priv->dp_modem_signals |= TIOCM_CTS;
				/* port must be open to use tty struct */
				if (rts)
					tty_port_tty_wakeup(&port->port);
			} else {
				priv->dp_modem_signals &= ~TIOCM_CTS;
				/* port must be open to use tty struct */
			}
			if (val & DIGI_READ_INPUT_SIGNALS_DSR)
				priv->dp_modem_signals |= TIOCM_DSR;
			else
				priv->dp_modem_signals &= ~TIOCM_DSR;
			if (val & DIGI_READ_INPUT_SIGNALS_RI)
				priv->dp_modem_signals |= TIOCM_RI;
			else
				priv->dp_modem_signals &= ~TIOCM_RI;
			if (val & DIGI_READ_INPUT_SIGNALS_DCD)
				priv->dp_modem_signals |= TIOCM_CD;
			else
				priv->dp_modem_signals &= ~TIOCM_CD;

			spin_unlock(&priv->dp_port_lock);
		} else if (opcode == DIGI_CMD_TRANSMIT_IDLE) {
			spin_lock(&priv->dp_port_lock);
			priv->dp_transmit_idle = 1;
			wake_up_interruptible(&priv->dp_transmit_idle_wait);
			spin_unlock(&priv->dp_port_lock);
		} else if (opcode == DIGI_CMD_IFLUSH_FIFO) {
			wake_up_interruptible(&priv->dp_flush_wait);
		}
		tty_kref_put(tty);
	}
	return 0;

}