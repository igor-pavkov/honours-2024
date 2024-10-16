void hci_chan_list_flush(struct hci_conn *conn)
{
	struct hci_chan *chan, *n;

	BT_DBG("hcon %p", conn);

	list_for_each_entry_safe(chan, n, &conn->chan_list, list)
		hci_chan_del(chan);
}