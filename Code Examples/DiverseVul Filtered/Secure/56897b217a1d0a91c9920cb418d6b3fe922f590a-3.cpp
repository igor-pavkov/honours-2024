static void hci_uart_tty_receive(struct tty_struct *tty, const u8 *data,
				 char *flags, int count)
{
	struct hci_uart *hu = tty->disc_data;

	if (!hu || tty != hu->tty)
		return;

	percpu_down_read(&hu->proto_lock);

	if (!test_bit(HCI_UART_PROTO_READY, &hu->flags)) {
		percpu_up_read(&hu->proto_lock);
		return;
	}

	/* It does not need a lock here as it is already protected by a mutex in
	 * tty caller
	 */
	hu->proto->recv(hu, data, count);
	percpu_up_read(&hu->proto_lock);

	if (hu->hdev)
		hu->hdev->stat.byte_rx += count;

	tty_unthrottle(tty);
}