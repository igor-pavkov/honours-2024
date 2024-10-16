static struct sk_buff *tipc_tlv_alloc(int size)
{
	int hdr_len;
	struct sk_buff *buf;

	size = TLV_SPACE(size);
	hdr_len = nlmsg_total_size(GENL_HDRLEN + TIPC_GENL_HDRLEN);

	buf = alloc_skb(hdr_len + size, GFP_KERNEL);
	if (!buf)
		return NULL;

	skb_reserve(buf, hdr_len);

	return buf;
}