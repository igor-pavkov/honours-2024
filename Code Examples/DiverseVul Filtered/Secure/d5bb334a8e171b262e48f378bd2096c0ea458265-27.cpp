static void hci_req_directed_advertising(struct hci_request *req,
					 struct hci_conn *conn)
{
	struct hci_dev *hdev = req->hdev;
	u8 own_addr_type;
	u8 enable;

	if (ext_adv_capable(hdev)) {
		struct hci_cp_le_set_ext_adv_params cp;
		bdaddr_t random_addr;

		/* Set require_privacy to false so that the remote device has a
		 * chance of identifying us.
		 */
		if (hci_get_random_address(hdev, false, conn_use_rpa(conn), NULL,
					   &own_addr_type, &random_addr) < 0)
			return;

		memset(&cp, 0, sizeof(cp));

		cp.evt_properties = cpu_to_le16(LE_LEGACY_ADV_DIRECT_IND);
		cp.own_addr_type = own_addr_type;
		cp.channel_map = hdev->le_adv_channel_map;
		cp.tx_power = HCI_TX_POWER_INVALID;
		cp.primary_phy = HCI_ADV_PHY_1M;
		cp.secondary_phy = HCI_ADV_PHY_1M;
		cp.handle = 0; /* Use instance 0 for directed adv */
		cp.own_addr_type = own_addr_type;
		cp.peer_addr_type = conn->dst_type;
		bacpy(&cp.peer_addr, &conn->dst);

		hci_req_add(req, HCI_OP_LE_SET_EXT_ADV_PARAMS, sizeof(cp), &cp);

		if (own_addr_type == ADDR_LE_DEV_RANDOM &&
		    bacmp(&random_addr, BDADDR_ANY) &&
		    bacmp(&random_addr, &hdev->random_addr)) {
			struct hci_cp_le_set_adv_set_rand_addr cp;

			memset(&cp, 0, sizeof(cp));

			cp.handle = 0;
			bacpy(&cp.bdaddr, &random_addr);

			hci_req_add(req,
				    HCI_OP_LE_SET_ADV_SET_RAND_ADDR,
				    sizeof(cp), &cp);
		}

		__hci_req_enable_ext_advertising(req);
	} else {
		struct hci_cp_le_set_adv_param cp;

		/* Clear the HCI_LE_ADV bit temporarily so that the
		 * hci_update_random_address knows that it's safe to go ahead
		 * and write a new random address. The flag will be set back on
		 * as soon as the SET_ADV_ENABLE HCI command completes.
		 */
		hci_dev_clear_flag(hdev, HCI_LE_ADV);

		/* Set require_privacy to false so that the remote device has a
		 * chance of identifying us.
		 */
		if (hci_update_random_address(req, false, conn_use_rpa(conn),
					      &own_addr_type) < 0)
			return;

		memset(&cp, 0, sizeof(cp));
		cp.type = LE_ADV_DIRECT_IND;
		cp.own_address_type = own_addr_type;
		cp.direct_addr_type = conn->dst_type;
		bacpy(&cp.direct_addr, &conn->dst);
		cp.channel_map = hdev->le_adv_channel_map;

		hci_req_add(req, HCI_OP_LE_SET_ADV_PARAM, sizeof(cp), &cp);

		enable = 0x01;
		hci_req_add(req, HCI_OP_LE_SET_ADV_ENABLE, sizeof(enable),
			    &enable);
	}

	conn->state = BT_CONNECT;
}