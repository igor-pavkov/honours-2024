void qdisc_calculate_pkt_len(struct sk_buff *skb, struct qdisc_size_table *stab)
{
	int pkt_len, slot;

	pkt_len = skb->len + stab->szopts.overhead;
	if (unlikely(!stab->szopts.tsize))
		goto out;

	slot = pkt_len + stab->szopts.cell_align;
	if (unlikely(slot < 0))
		slot = 0;

	slot >>= stab->szopts.cell_log;
	if (likely(slot < stab->szopts.tsize))
		pkt_len = stab->data[slot];
	else
		pkt_len = stab->data[stab->szopts.tsize - 1] *
				(slot / stab->szopts.tsize) +
				stab->data[slot % stab->szopts.tsize];

	pkt_len <<= stab->szopts.size_log;
out:
	if (unlikely(pkt_len < 1))
		pkt_len = 1;
	qdisc_skb_cb(skb)->pkt_len = pkt_len;
}