static int tipc_add_tlv(struct sk_buff *skb, u16 type, void *data, u16 len)
{
	struct tlv_desc *tlv = (struct tlv_desc *)skb_tail_pointer(skb);

	if (tipc_skb_tailroom(skb) < TLV_SPACE(len))
		return -EMSGSIZE;

	skb_put(skb, TLV_SPACE(len));
	tlv->tlv_type = htons(type);
	tlv->tlv_len = htons(TLV_LENGTH(len));
	if (len && data)
		memcpy(TLV_DATA(tlv), data, len);

	return 0;
}