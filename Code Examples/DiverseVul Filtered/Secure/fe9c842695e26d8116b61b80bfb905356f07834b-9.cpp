static struct sk_buff *llcp_add_tlv(struct sk_buff *pdu, u8 *tlv,
				    u8 tlv_length)
{
	/* XXX Add an skb length check */

	if (tlv == NULL)
		return NULL;

	skb_put_data(pdu, tlv, tlv_length);

	return pdu;
}