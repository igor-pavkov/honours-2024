static int sco_connect_cfm(struct hci_conn *hcon, __u8 status)
{
	BT_DBG("hcon %p bdaddr %s status %d", hcon, batostr(&hcon->dst), status);

	if (hcon->type != SCO_LINK && hcon->type != ESCO_LINK)
		return -EINVAL;

	if (!status) {
		struct sco_conn *conn;

		conn = sco_conn_add(hcon, status);
		if (conn)
			sco_conn_ready(conn);
	} else
		sco_conn_del(hcon, bt_err(status));

	return 0;
}