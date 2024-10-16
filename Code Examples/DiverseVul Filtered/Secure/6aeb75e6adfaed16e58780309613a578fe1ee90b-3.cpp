static void edge_set_termios(struct tty_struct *tty,
		struct usb_serial_port *port, struct ktermios *old_termios)
{
	struct edgeport_port *edge_port = usb_get_serial_port_data(port);

	if (edge_port == NULL)
		return;
	/* change the port settings to the new ones specified */
	change_port_settings(tty, edge_port, old_termios);
}