static void hci_conn_timeout(struct work_struct *work)
{
	struct hci_conn *conn = container_of(work, struct hci_conn,
					     disc_work.work);
	int refcnt = atomic_read(&conn->refcnt);

	BT_DBG("hcon %p state %s", conn, state_to_string(conn->state));

	WARN_ON(refcnt < 0);

	/* FIXME: It was observed that in pairing failed scenario, refcnt
	 * drops below 0. Probably this is because l2cap_conn_del calls
	 * l2cap_chan_del for each channel, and inside l2cap_chan_del conn is
	 * dropped. After that loop hci_chan_del is called which also drops
	 * conn. For now make sure that ACL is alive if refcnt is higher then 0,
	 * otherwise drop it.
	 */
	if (refcnt > 0)
		return;

	/* LE connections in scanning state need special handling */
	if (conn->state == BT_CONNECT && conn->type == LE_LINK &&
	    test_bit(HCI_CONN_SCANNING, &conn->flags)) {
		hci_connect_le_scan_remove(conn);
		return;
	}

	hci_abort_conn(conn, hci_proto_disconn_ind(conn));
}