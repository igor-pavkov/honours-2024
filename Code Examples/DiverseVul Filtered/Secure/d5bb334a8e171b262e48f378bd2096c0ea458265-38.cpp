static bool conn_use_rpa(struct hci_conn *conn)
{
	struct hci_dev *hdev = conn->hdev;

	return hci_dev_test_flag(hdev, HCI_PRIVACY);
}