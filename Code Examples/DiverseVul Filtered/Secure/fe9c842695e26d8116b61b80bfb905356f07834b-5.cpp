static struct sk_buff *llcp_add_header(struct sk_buff *pdu,
				       u8 dsap, u8 ssap, u8 ptype)
{
	u8 header[2];

	pr_debug("ptype 0x%x dsap 0x%x ssap 0x%x\n", ptype, dsap, ssap);

	header[0] = (u8)((dsap << 2) | (ptype >> 2));
	header[1] = (u8)((ptype << 6) | ssap);

	pr_debug("header 0x%x 0x%x\n", header[0], header[1]);

	skb_put_data(pdu, header, LLCP_HEADER_SIZE);

	return pdu;
}