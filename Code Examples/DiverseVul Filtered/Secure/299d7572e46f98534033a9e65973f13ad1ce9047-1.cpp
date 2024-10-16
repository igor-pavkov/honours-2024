static void usb_console_write(struct console *co,
					const char *buf, unsigned count)
{
	static struct usbcons_info *info = &usbcons_info;
	struct usb_serial_port *port = info->port;
	struct usb_serial *serial;
	int retval = -ENODEV;

	if (!port || port->serial->dev->state == USB_STATE_NOTATTACHED)
		return;
	serial = port->serial;

	if (count == 0)
		return;

	dev_dbg(&port->dev, "%s - %d byte(s)\n", __func__, count);

	if (!port->port.console) {
		dev_dbg(&port->dev, "%s - port not opened\n", __func__);
		return;
	}

	while (count) {
		unsigned int i;
		unsigned int lf;
		/* search for LF so we can insert CR if necessary */
		for (i = 0, lf = 0 ; i < count ; i++) {
			if (*(buf + i) == 10) {
				lf = 1;
				i++;
				break;
			}
		}
		/* pass on to the driver specific version of this function if
		   it is available */
		retval = serial->type->write(NULL, port, buf, i);
		dev_dbg(&port->dev, "%s - write: %d\n", __func__, retval);
		if (lf) {
			/* append CR after LF */
			unsigned char cr = 13;
			retval = serial->type->write(NULL, port, &cr, 1);
			dev_dbg(&port->dev, "%s - write cr: %d\n",
							__func__, retval);
		}
		buf += i;
		count -= i;
	}
}