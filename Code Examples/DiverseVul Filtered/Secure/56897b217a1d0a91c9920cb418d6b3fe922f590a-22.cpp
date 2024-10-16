static int hci_uart_open(struct hci_dev *hdev)
{
	BT_DBG("%s %p", hdev->name, hdev);

	/* Undo clearing this from hci_uart_close() */
	hdev->flush = hci_uart_flush;

	return 0;
}