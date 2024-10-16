int nfc_llcp_send_disconnect(struct nfc_llcp_sock *sock)
{
	struct sk_buff *skb;
	struct nfc_dev *dev;
	struct nfc_llcp_local *local;

	pr_debug("Sending DISC\n");

	local = sock->local;
	if (local == NULL)
		return -ENODEV;

	dev = sock->dev;
	if (dev == NULL)
		return -ENODEV;

	skb = llcp_allocate_pdu(sock, LLCP_PDU_DISC, 0);
	if (skb == NULL)
		return -ENOMEM;

	skb_queue_tail(&local->tx_queue, skb);

	return 0;
}