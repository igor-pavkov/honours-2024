static void hci_req_add_le_create_conn(struct hci_request *req,
				       struct hci_conn *conn,
				       bdaddr_t *direct_rpa)
{
	struct hci_dev *hdev = conn->hdev;
	u8 own_addr_type;

	/* If direct address was provided we use it instead of current
	 * address.
	 */
	if (direct_rpa) {
		if (bacmp(&req->hdev->random_addr, direct_rpa))
			hci_req_add(req, HCI_OP_LE_SET_RANDOM_ADDR, 6,
								direct_rpa);

		/* direct address is always RPA */
		own_addr_type = ADDR_LE_DEV_RANDOM;
	} else {
		/* Update random address, but set require_privacy to false so
		 * that we never connect with an non-resolvable address.
		 */
		if (hci_update_random_address(req, false, conn_use_rpa(conn),
					      &own_addr_type))
			return;
	}

	if (use_ext_conn(hdev)) {
		struct hci_cp_le_ext_create_conn *cp;
		struct hci_cp_le_ext_conn_param *p;
		u8 data[sizeof(*cp) + sizeof(*p) * 3];
		u32 plen;

		cp = (void *) data;
		p = (void *) cp->data;

		memset(cp, 0, sizeof(*cp));

		bacpy(&cp->peer_addr, &conn->dst);
		cp->peer_addr_type = conn->dst_type;
		cp->own_addr_type = own_addr_type;

		plen = sizeof(*cp);

		if (scan_1m(hdev)) {
			cp->phys |= LE_SCAN_PHY_1M;
			set_ext_conn_params(conn, p);

			p++;
			plen += sizeof(*p);
		}

		if (scan_2m(hdev)) {
			cp->phys |= LE_SCAN_PHY_2M;
			set_ext_conn_params(conn, p);

			p++;
			plen += sizeof(*p);
		}

		if (scan_coded(hdev)) {
			cp->phys |= LE_SCAN_PHY_CODED;
			set_ext_conn_params(conn, p);

			plen += sizeof(*p);
		}

		hci_req_add(req, HCI_OP_LE_EXT_CREATE_CONN, plen, data);

	} else {
		struct hci_cp_le_create_conn cp;

		memset(&cp, 0, sizeof(cp));

		/* Set window to be the same value as the interval to enable
		 * continuous scanning.
		 */
		cp.scan_interval = cpu_to_le16(hdev->le_scan_interval);
		cp.scan_window = cp.scan_interval;

		bacpy(&cp.peer_addr, &conn->dst);
		cp.peer_addr_type = conn->dst_type;
		cp.own_address_type = own_addr_type;
		cp.conn_interval_min = cpu_to_le16(conn->le_conn_min_interval);
		cp.conn_interval_max = cpu_to_le16(conn->le_conn_max_interval);
		cp.conn_latency = cpu_to_le16(conn->le_conn_latency);
		cp.supervision_timeout = cpu_to_le16(conn->le_supv_timeout);
		cp.min_ce_len = cpu_to_le16(0x0000);
		cp.max_ce_len = cpu_to_le16(0x0000);

		hci_req_add(req, HCI_OP_LE_CREATE_CONN, sizeof(cp), &cp);
	}

	conn->state = BT_CONNECT;
	clear_bit(HCI_CONN_SCANNING, &conn->flags);
}