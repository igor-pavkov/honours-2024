void hci_uart_set_flow_control(struct hci_uart *hu, bool enable)
{
	struct tty_struct *tty = hu->tty;
	struct ktermios ktermios;
	int status;
	unsigned int set = 0;
	unsigned int clear = 0;

	if (hu->serdev) {
		serdev_device_set_flow_control(hu->serdev, !enable);
		serdev_device_set_rts(hu->serdev, !enable);
		return;
	}

	if (enable) {
		/* Disable hardware flow control */
		ktermios = tty->termios;
		ktermios.c_cflag &= ~CRTSCTS;
		status = tty_set_termios(tty, &ktermios);
		BT_DBG("Disabling hardware flow control: %s",
		       status ? "failed" : "success");

		/* Clear RTS to prevent the device from sending */
		/* Most UARTs need OUT2 to enable interrupts */
		status = tty->driver->ops->tiocmget(tty);
		BT_DBG("Current tiocm 0x%x", status);

		set &= ~(TIOCM_OUT2 | TIOCM_RTS);
		clear = ~set;
		set &= TIOCM_DTR | TIOCM_RTS | TIOCM_OUT1 |
		       TIOCM_OUT2 | TIOCM_LOOP;
		clear &= TIOCM_DTR | TIOCM_RTS | TIOCM_OUT1 |
			 TIOCM_OUT2 | TIOCM_LOOP;
		status = tty->driver->ops->tiocmset(tty, set, clear);
		BT_DBG("Clearing RTS: %s", status ? "failed" : "success");
	} else {
		/* Set RTS to allow the device to send again */
		status = tty->driver->ops->tiocmget(tty);
		BT_DBG("Current tiocm 0x%x", status);

		set |= (TIOCM_OUT2 | TIOCM_RTS);
		clear = ~set;
		set &= TIOCM_DTR | TIOCM_RTS | TIOCM_OUT1 |
		       TIOCM_OUT2 | TIOCM_LOOP;
		clear &= TIOCM_DTR | TIOCM_RTS | TIOCM_OUT1 |
			 TIOCM_OUT2 | TIOCM_LOOP;
		status = tty->driver->ops->tiocmset(tty, set, clear);
		BT_DBG("Setting RTS: %s", status ? "failed" : "success");

		/* Re-enable hardware flow control */
		ktermios = tty->termios;
		ktermios.c_cflag |= CRTSCTS;
		status = tty_set_termios(tty, &ktermios);
		BT_DBG("Enabling hardware flow control: %s",
		       status ? "failed" : "success");
	}
}