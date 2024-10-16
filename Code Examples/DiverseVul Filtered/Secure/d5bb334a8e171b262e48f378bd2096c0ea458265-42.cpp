void hci_le_conn_failed(struct hci_conn *conn, u8 status)
{
	struct hci_dev *hdev = conn->hdev;
	struct hci_conn_params *params;

	params = hci_pend_le_action_lookup(&hdev->pend_le_conns, &conn->dst,
					   conn->dst_type);
	if (params && params->conn) {
		hci_conn_drop(params->conn);
		hci_conn_put(params->conn);
		params->conn = NULL;
	}

	conn->state = BT_CLOSED;

	/* If the status indicates successful cancellation of
	 * the attempt (i.e. Unkown Connection Id) there's no point of
	 * notifying failure since we'll go back to keep trying to
	 * connect. The only exception is explicit connect requests
	 * where a timeout + cancel does indicate an actual failure.
	 */
	if (status != HCI_ERROR_UNKNOWN_CONN_ID ||
	    (params && params->explicit_connect))
		mgmt_connect_failed(hdev, &conn->dst, conn->type,
				    conn->dst_type, status);

	hci_connect_cfm(conn, status);

	hci_conn_del(conn);

	/* Since we may have temporarily stopped the background scanning in
	 * favor of connection establishment, we should restart it.
	 */
	hci_update_background_scan(hdev);

	/* Re-enable advertising in case this was a failed connection
	 * attempt as a peripheral.
	 */
	hci_req_reenable_advertising(hdev);
}