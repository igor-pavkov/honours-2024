static void mct_u232_break_ctl(struct tty_struct *tty, int break_state)
{
	struct usb_serial_port *port = tty->driver_data;
	struct usb_serial *serial = port->serial;
	struct mct_u232_private *priv =
				(struct mct_u232_private *)port->private;
	unsigned char lcr = priv->last_lcr;

	dev_dbg(&port->dev, "%s - state=%d\n", __func__, break_state);

	/* LOCKING */
	if (break_state)
		lcr |= MCT_U232_SET_BREAK;

	mct_u232_set_line_ctrl(serial, lcr);
}