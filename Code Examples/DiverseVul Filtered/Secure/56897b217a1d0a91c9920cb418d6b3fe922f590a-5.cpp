void hci_uart_set_baudrate(struct hci_uart *hu, unsigned int speed)
{
	struct tty_struct *tty = hu->tty;
	struct ktermios ktermios;

	ktermios = tty->termios;
	ktermios.c_cflag &= ~CBAUD;
	tty_termios_encode_baud_rate(&ktermios, speed, speed);

	/* tty_set_termios() return not checked as it is always 0 */
	tty_set_termios(tty, &ktermios);

	BT_DBG("%s: New tty speeds: %d/%d", hu->hdev->name,
	       tty->termios.c_ispeed, tty->termios.c_ospeed);
}