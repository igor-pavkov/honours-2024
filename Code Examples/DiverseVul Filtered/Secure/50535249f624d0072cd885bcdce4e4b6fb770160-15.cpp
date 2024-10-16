static struct sk_buff *qrtr_alloc_ctrl_packet(struct qrtr_ctrl_pkt **pkt,
					      gfp_t flags)
{
	const int pkt_len = sizeof(struct qrtr_ctrl_pkt);
	struct sk_buff *skb;

	skb = alloc_skb(QRTR_HDR_MAX_SIZE + pkt_len, flags);
	if (!skb)
		return NULL;

	skb_reserve(skb, QRTR_HDR_MAX_SIZE);
	*pkt = skb_put_zero(skb, pkt_len);

	return skb;
}