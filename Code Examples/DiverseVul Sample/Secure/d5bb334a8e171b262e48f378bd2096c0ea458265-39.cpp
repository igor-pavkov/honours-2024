static void set_ext_conn_params(struct hci_conn *conn,
				struct hci_cp_le_ext_conn_param *p)
{
	struct hci_dev *hdev = conn->hdev;

	memset(p, 0, sizeof(*p));

	/* Set window to be the same value as the interval to
	 * enable continuous scanning.
	 */
	p->scan_interval = cpu_to_le16(hdev->le_scan_interval);
	p->scan_window = p->scan_interval;
	p->conn_interval_min = cpu_to_le16(conn->le_conn_min_interval);
	p->conn_interval_max = cpu_to_le16(conn->le_conn_max_interval);
	p->conn_latency = cpu_to_le16(conn->le_conn_latency);
	p->supervision_timeout = cpu_to_le16(conn->le_supv_timeout);
	p->min_ce_len = cpu_to_le16(0x0000);
	p->max_ce_len = cpu_to_le16(0x0000);
}