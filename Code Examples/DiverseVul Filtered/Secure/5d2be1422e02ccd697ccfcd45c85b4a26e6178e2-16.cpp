static int tipc_tlv_sprintf(struct sk_buff *skb, const char *fmt, ...)
{
	int n;
	u16 len;
	u32 rem;
	char *buf;
	struct tlv_desc *tlv;
	va_list args;

	rem = tipc_skb_tailroom(skb);

	tlv = (struct tlv_desc *)skb->data;
	len = TLV_GET_LEN(tlv);
	buf = TLV_DATA(tlv) + len;

	va_start(args, fmt);
	n = vscnprintf(buf, rem, fmt, args);
	va_end(args);

	TLV_SET_LEN(tlv, n + len);
	skb_put(skb, n);

	return n;
}