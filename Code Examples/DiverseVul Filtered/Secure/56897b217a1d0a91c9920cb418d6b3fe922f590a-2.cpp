static void hci_uart_tty_close(struct tty_struct *tty)
{
	struct hci_uart *hu = tty->disc_data;
	struct hci_dev *hdev;

	BT_DBG("tty %p", tty);

	/* Detach from the tty */
	tty->disc_data = NULL;

	if (!hu)
		return;

	hdev = hu->hdev;
	if (hdev)
		hci_uart_close(hdev);

	if (test_bit(HCI_UART_PROTO_READY, &hu->flags)) {
		percpu_down_write(&hu->proto_lock);
		clear_bit(HCI_UART_PROTO_READY, &hu->flags);
		percpu_up_write(&hu->proto_lock);

		cancel_work_sync(&hu->write_work);

		if (hdev) {
			if (test_bit(HCI_UART_REGISTERED, &hu->flags))
				hci_unregister_dev(hdev);
			hci_free_dev(hdev);
		}
		hu->proto->close(hu);
	}
	clear_bit(HCI_UART_PROTO_SET, &hu->flags);

	percpu_free_rwsem(&hu->proto_lock);

	kfree(hu);
}