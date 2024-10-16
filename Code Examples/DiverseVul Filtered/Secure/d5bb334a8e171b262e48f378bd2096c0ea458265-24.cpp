static void create_le_conn_complete(struct hci_dev *hdev, u8 status, u16 opcode)
{
	struct hci_conn *conn;

	hci_dev_lock(hdev);

	conn = hci_lookup_le_connect(hdev);

	if (!status) {
		hci_connect_le_scan_cleanup(conn);
		goto done;
	}

	bt_dev_err(hdev, "request failed to create LE connection: "
		   "status 0x%2.2x", status);

	if (!conn)
		goto done;

	hci_le_conn_failed(conn, status);

done:
	hci_dev_unlock(hdev);
}