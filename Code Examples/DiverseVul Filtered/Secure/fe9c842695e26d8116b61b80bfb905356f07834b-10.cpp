int nfc_llcp_send_dm(struct nfc_llcp_local *local, u8 ssap, u8 dsap, u8 reason)
{
	struct sk_buff *skb;
	struct nfc_dev *dev;
	u16 size = 1; /* Reason code */

	pr_debug("Sending DM reason 0x%x\n", reason);

	if (local == NULL)
		return -ENODEV;

	dev = local->dev;
	if (dev == NULL)
		return -ENODEV;

	size += LLCP_HEADER_SIZE;
	size += dev->tx_headroom + dev->tx_tailroom + NFC_HEADER_SIZE;

	skb = alloc_skb(size, GFP_KERNEL);
	if (skb == NULL)
		return -ENOMEM;

	skb_reserve(skb, dev->tx_headroom + NFC_HEADER_SIZE);

	skb = llcp_add_header(skb, dsap, ssap, LLCP_PDU_DM);

	skb_put_data(skb, &reason, 1);

	skb_queue_head(&local->tx_queue, skb);

	return 0;
}