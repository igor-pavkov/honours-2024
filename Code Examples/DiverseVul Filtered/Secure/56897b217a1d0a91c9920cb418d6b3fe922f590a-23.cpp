static void hci_uart_tty_wakeup(struct tty_struct *tty)
{
	struct hci_uart *hu = tty->disc_data;

	BT_DBG("");

	if (!hu)
		return;

	clear_bit(TTY_DO_WRITE_WAKEUP, &tty->flags);

	if (tty != hu->tty)
		return;

	if (test_bit(HCI_UART_PROTO_READY, &hu->flags))
		hci_uart_tx_wakeup(hu);
}