static void le_conn_timeout(struct work_struct *work)
{
	struct hci_conn *conn = container_of(work, struct hci_conn,
					     le_conn_timeout.work);
	struct hci_dev *hdev = conn->hdev;

	BT_DBG("");

	/* We could end up here due to having done directed advertising,
	 * so clean up the state if necessary. This should however only
	 * happen with broken hardware or if low duty cycle was used
	 * (which doesn't have a timeout of its own).
	 */
	if (conn->role == HCI_ROLE_SLAVE) {
		u8 enable = 0x00;
		hci_send_cmd(hdev, HCI_OP_LE_SET_ADV_ENABLE, sizeof(enable),
			     &enable);
		hci_le_conn_failed(conn, HCI_ERROR_ADVERTISING_TIMEOUT);
		return;
	}

	hci_abort_conn(conn, HCI_ERROR_REMOTE_USER_TERM);
}