int hci_uart_register_proto(const struct hci_uart_proto *p)
{
	if (p->id >= HCI_UART_MAX_PROTO)
		return -EINVAL;

	if (hup[p->id])
		return -EEXIST;

	hup[p->id] = p;

	BT_INFO("HCI UART protocol %s registered", p->name);

	return 0;
}