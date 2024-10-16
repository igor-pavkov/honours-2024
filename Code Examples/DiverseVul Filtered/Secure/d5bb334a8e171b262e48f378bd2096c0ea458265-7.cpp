struct hci_dev *hci_get_route(bdaddr_t *dst, bdaddr_t *src, uint8_t src_type)
{
	int use_src = bacmp(src, BDADDR_ANY);
	struct hci_dev *hdev = NULL, *d;

	BT_DBG("%pMR -> %pMR", src, dst);

	read_lock(&hci_dev_list_lock);

	list_for_each_entry(d, &hci_dev_list, list) {
		if (!test_bit(HCI_UP, &d->flags) ||
		    hci_dev_test_flag(d, HCI_USER_CHANNEL) ||
		    d->dev_type != HCI_PRIMARY)
			continue;

		/* Simple routing:
		 *   No source address - find interface with bdaddr != dst
		 *   Source address    - find interface with bdaddr == src
		 */

		if (use_src) {
			bdaddr_t id_addr;
			u8 id_addr_type;

			if (src_type == BDADDR_BREDR) {
				if (!lmp_bredr_capable(d))
					continue;
				bacpy(&id_addr, &d->bdaddr);
				id_addr_type = BDADDR_BREDR;
			} else {
				if (!lmp_le_capable(d))
					continue;

				hci_copy_identity_address(d, &id_addr,
							  &id_addr_type);

				/* Convert from HCI to three-value type */
				if (id_addr_type == ADDR_LE_DEV_PUBLIC)
					id_addr_type = BDADDR_LE_PUBLIC;
				else
					id_addr_type = BDADDR_LE_RANDOM;
			}

			if (!bacmp(&id_addr, src) && id_addr_type == src_type) {
				hdev = d; break;
			}
		} else {
			if (bacmp(&d->bdaddr, dst)) {
				hdev = d; break;
			}
		}
	}

	if (hdev)
		hdev = hci_dev_hold(hdev);

	read_unlock(&hci_dev_list_lock);
	return hdev;
}