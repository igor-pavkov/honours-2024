static bool is_connected(struct hci_dev *hdev, bdaddr_t *addr, u8 type)
{
	struct hci_conn *conn;

	conn = hci_conn_hash_lookup_le(hdev, addr, type);
	if (!conn)
		return false;

	if (conn->state != BT_CONNECTED)
		return false;

	return true;
}