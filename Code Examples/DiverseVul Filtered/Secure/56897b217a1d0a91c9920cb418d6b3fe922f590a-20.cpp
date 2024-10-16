static int hci_uart_close(struct hci_dev *hdev)
{
	BT_DBG("hdev %p", hdev);

	hci_uart_flush(hdev);
	hdev->flush = NULL;
	return 0;
}