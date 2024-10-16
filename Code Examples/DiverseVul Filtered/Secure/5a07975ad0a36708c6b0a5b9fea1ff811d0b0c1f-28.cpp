static int digi_read_inb_callback(struct urb *urb)
{
	struct usb_serial_port *port = urb->context;
	struct digi_port *priv = usb_get_serial_port_data(port);
	int opcode = ((unsigned char *)urb->transfer_buffer)[0];
	int len = ((unsigned char *)urb->transfer_buffer)[1];
	int port_status = ((unsigned char *)urb->transfer_buffer)[2];
	unsigned char *data = ((unsigned char *)urb->transfer_buffer) + 3;
	int flag, throttled;
	int status = urb->status;

	/* do not process callbacks on closed ports */
	/* but do continue the read chain */
	if (urb->status == -ENOENT)
		return 0;

	/* short/multiple packet check */
	if (urb->actual_length != len + 2) {
		dev_err(&port->dev, "%s: INCOMPLETE OR MULTIPLE PACKET, "
			"status=%d, port=%d, opcode=%d, len=%d, "
			"actual_length=%d, status=%d\n", __func__, status,
			priv->dp_port_num, opcode, len, urb->actual_length,
			port_status);
		return -1;
	}

	spin_lock(&priv->dp_port_lock);

	/* check for throttle; if set, do not resubmit read urb */
	/* indicate the read chain needs to be restarted on unthrottle */
	throttled = priv->dp_throttled;
	if (throttled)
		priv->dp_throttle_restart = 1;

	/* receive data */
	if (opcode == DIGI_CMD_RECEIVE_DATA) {
		/* get flag from port_status */
		flag = 0;

		/* overrun is special, not associated with a char */
		if (port_status & DIGI_OVERRUN_ERROR)
			tty_insert_flip_char(&port->port, 0, TTY_OVERRUN);

		/* break takes precedence over parity, */
		/* which takes precedence over framing errors */
		if (port_status & DIGI_BREAK_ERROR)
			flag = TTY_BREAK;
		else if (port_status & DIGI_PARITY_ERROR)
			flag = TTY_PARITY;
		else if (port_status & DIGI_FRAMING_ERROR)
			flag = TTY_FRAME;

		/* data length is len-1 (one byte of len is port_status) */
		--len;
		if (len > 0) {
			tty_insert_flip_string_fixed_flag(&port->port, data,
					flag, len);
			tty_flip_buffer_push(&port->port);
		}
	}
	spin_unlock(&priv->dp_port_lock);

	if (opcode == DIGI_CMD_RECEIVE_DISABLE)
		dev_dbg(&port->dev, "%s: got RECEIVE_DISABLE\n", __func__);
	else if (opcode != DIGI_CMD_RECEIVE_DATA)
		dev_dbg(&port->dev, "%s: unknown opcode: %d\n", __func__, opcode);

	return throttled ? 1 : 0;

}