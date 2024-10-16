int nfc_llcp_send_symm(struct nfc_dev *dev)
{
	struct sk_buff *skb;
	struct nfc_llcp_local *local;
	u16 size = 0;

	pr_debug("Sending SYMM\n");

	local = nfc_llcp_find_local(dev);
	if (local == NULL)
		return -ENODEV;

	size += LLCP_HEADER_SIZE;
	size += dev->tx_headroom + dev->tx_tailroom + NFC_HEADER_SIZE;

	skb = alloc_skb(size, GFP_KERNEL);
	if (skb == NULL)
		return -ENOMEM;

	skb_reserve(skb, dev->tx_headroom + NFC_HEADER_SIZE);

	skb = llcp_add_header(skb, 0, 0, LLCP_PDU_SYMM);

	__net_timestamp(skb);

	nfc_llcp_send_to_raw_sock(local, skb, NFC_DIRECTION_TX);

	return nfc_data_exchange(dev, local->target_idx, skb,
				 nfc_llcp_recv, local);
}