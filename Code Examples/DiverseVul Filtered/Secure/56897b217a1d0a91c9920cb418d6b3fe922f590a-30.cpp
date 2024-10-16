void hci_uart_init_work(struct work_struct *work)
{
	struct hci_uart *hu = container_of(work, struct hci_uart, init_ready);
	int err;
	struct hci_dev *hdev;

	if (!test_and_clear_bit(HCI_UART_INIT_PENDING, &hu->hdev_flags))
		return;

	err = hci_register_dev(hu->hdev);
	if (err < 0) {
		BT_ERR("Can't register HCI device");
		clear_bit(HCI_UART_PROTO_READY, &hu->flags);
		hu->proto->close(hu);
		hdev = hu->hdev;
		hu->hdev = NULL;
		hci_free_dev(hdev);
		return;
	}

	set_bit(HCI_UART_REGISTERED, &hu->flags);
}