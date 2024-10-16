static int xfrm_smark_put(struct sk_buff *skb, struct xfrm_mark *m)
{
	int ret = 0;

	if (m->v | m->m) {
		ret = nla_put_u32(skb, XFRMA_SET_MARK, m->v);
		if (!ret)
			ret = nla_put_u32(skb, XFRMA_SET_MARK_MASK, m->m);
	}
	return ret;
}