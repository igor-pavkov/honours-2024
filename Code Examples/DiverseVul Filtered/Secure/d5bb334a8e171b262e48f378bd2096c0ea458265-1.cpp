struct hci_conn *hci_connect_le(struct hci_dev *hdev, bdaddr_t *dst,
				u8 dst_type, u8 sec_level, u16 conn_timeout,
				u8 role, bdaddr_t *direct_rpa)
{
	struct hci_conn_params *params;
	struct hci_conn *conn;
	struct smp_irk *irk;
	struct hci_request req;
	int err;

	/* Let's make sure that le is enabled.*/
	if (!hci_dev_test_flag(hdev, HCI_LE_ENABLED)) {
		if (lmp_le_capable(hdev))
			return ERR_PTR(-ECONNREFUSED);

		return ERR_PTR(-EOPNOTSUPP);
	}

	/* Since the controller supports only one LE connection attempt at a
	 * time, we return -EBUSY if there is any connection attempt running.
	 */
	if (hci_lookup_le_connect(hdev))
		return ERR_PTR(-EBUSY);

	/* If there's already a connection object but it's not in
	 * scanning state it means it must already be established, in
	 * which case we can't do anything else except report a failure
	 * to connect.
	 */
	conn = hci_conn_hash_lookup_le(hdev, dst, dst_type);
	if (conn && !test_bit(HCI_CONN_SCANNING, &conn->flags)) {
		return ERR_PTR(-EBUSY);
	}

	/* When given an identity address with existing identity
	 * resolving key, the connection needs to be established
	 * to a resolvable random address.
	 *
	 * Storing the resolvable random address is required here
	 * to handle connection failures. The address will later
	 * be resolved back into the original identity address
	 * from the connect request.
	 */
	irk = hci_find_irk_by_addr(hdev, dst, dst_type);
	if (irk && bacmp(&irk->rpa, BDADDR_ANY)) {
		dst = &irk->rpa;
		dst_type = ADDR_LE_DEV_RANDOM;
	}

	if (conn) {
		bacpy(&conn->dst, dst);
	} else {
		conn = hci_conn_add(hdev, LE_LINK, dst, role);
		if (!conn)
			return ERR_PTR(-ENOMEM);
		hci_conn_hold(conn);
		conn->pending_sec_level = sec_level;
	}

	conn->dst_type = dst_type;
	conn->sec_level = BT_SECURITY_LOW;
	conn->conn_timeout = conn_timeout;

	hci_req_init(&req, hdev);

	/* Disable advertising if we're active. For master role
	 * connections most controllers will refuse to connect if
	 * advertising is enabled, and for slave role connections we
	 * anyway have to disable it in order to start directed
	 * advertising.
	 */
	if (hci_dev_test_flag(hdev, HCI_LE_ADV)) {
		u8 enable = 0x00;
		hci_req_add(&req, HCI_OP_LE_SET_ADV_ENABLE, sizeof(enable),
			    &enable);
	}

	/* If requested to connect as slave use directed advertising */
	if (conn->role == HCI_ROLE_SLAVE) {
		/* If we're active scanning most controllers are unable
		 * to initiate advertising. Simply reject the attempt.
		 */
		if (hci_dev_test_flag(hdev, HCI_LE_SCAN) &&
		    hdev->le_scan_type == LE_SCAN_ACTIVE) {
			hci_req_purge(&req);
			hci_conn_del(conn);
			return ERR_PTR(-EBUSY);
		}

		hci_req_directed_advertising(&req, conn);
		goto create_conn;
	}

	params = hci_conn_params_lookup(hdev, &conn->dst, conn->dst_type);
	if (params) {
		conn->le_conn_min_interval = params->conn_min_interval;
		conn->le_conn_max_interval = params->conn_max_interval;
		conn->le_conn_latency = params->conn_latency;
		conn->le_supv_timeout = params->supervision_timeout;
	} else {
		conn->le_conn_min_interval = hdev->le_conn_min_interval;
		conn->le_conn_max_interval = hdev->le_conn_max_interval;
		conn->le_conn_latency = hdev->le_conn_latency;
		conn->le_supv_timeout = hdev->le_supv_timeout;
	}

	/* If controller is scanning, we stop it since some controllers are
	 * not able to scan and connect at the same time. Also set the
	 * HCI_LE_SCAN_INTERRUPTED flag so that the command complete
	 * handler for scan disabling knows to set the correct discovery
	 * state.
	 */
	if (hci_dev_test_flag(hdev, HCI_LE_SCAN)) {
		hci_req_add_le_scan_disable(&req);
		hci_dev_set_flag(hdev, HCI_LE_SCAN_INTERRUPTED);
	}

	hci_req_add_le_create_conn(&req, conn, direct_rpa);

create_conn:
	err = hci_req_run(&req, create_le_conn_complete);
	if (err) {
		hci_conn_del(conn);
		return ERR_PTR(err);
	}

	return conn;
}