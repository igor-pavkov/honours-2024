u8 hci_le_conn_update(struct hci_conn *conn, u16 min, u16 max, u16 latency,
		      u16 to_multiplier)
{
	struct hci_dev *hdev = conn->hdev;
	struct hci_conn_params *params;
	struct hci_cp_le_conn_update cp;

	hci_dev_lock(hdev);

	params = hci_conn_params_lookup(hdev, &conn->dst, conn->dst_type);
	if (params) {
		params->conn_min_interval = min;
		params->conn_max_interval = max;
		params->conn_latency = latency;
		params->supervision_timeout = to_multiplier;
	}

	hci_dev_unlock(hdev);

	memset(&cp, 0, sizeof(cp));
	cp.handle		= cpu_to_le16(conn->handle);
	cp.conn_interval_min	= cpu_to_le16(min);
	cp.conn_interval_max	= cpu_to_le16(max);
	cp.conn_latency		= cpu_to_le16(latency);
	cp.supervision_timeout	= cpu_to_le16(to_multiplier);
	cp.min_ce_len		= cpu_to_le16(0x0000);
	cp.max_ce_len		= cpu_to_le16(0x0000);

	hci_send_cmd(hdev, HCI_OP_LE_CONN_UPDATE, sizeof(cp), &cp);

	if (params)
		return 0x01;

	return 0x00;
}