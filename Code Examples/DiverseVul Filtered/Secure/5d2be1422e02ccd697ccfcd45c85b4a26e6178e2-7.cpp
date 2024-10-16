static void tipc_tlv_init(struct sk_buff *skb, u16 type)
{
	struct tlv_desc *tlv = (struct tlv_desc *)skb->data;

	TLV_SET_LEN(tlv, 0);
	TLV_SET_TYPE(tlv, type);
	skb_put(skb, sizeof(struct tlv_desc));
}