static int sco_disconn_cfm(struct hci_conn *hcon, __u8 reason)
{
	BT_DBG("hcon %p reason %d", hcon, reason);

	if (hcon->type != SCO_LINK && hcon->type != ESCO_LINK)
		return -EINVAL;

	sco_conn_del(hcon, bt_err(reason));

	return 0;
}