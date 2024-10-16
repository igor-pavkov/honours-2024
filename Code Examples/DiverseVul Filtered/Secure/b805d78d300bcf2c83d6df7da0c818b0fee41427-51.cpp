static void xfrm_smark_init(struct nlattr **attrs, struct xfrm_mark *m)
{
	if (attrs[XFRMA_SET_MARK]) {
		m->v = nla_get_u32(attrs[XFRMA_SET_MARK]);
		if (attrs[XFRMA_SET_MARK_MASK])
			m->m = nla_get_u32(attrs[XFRMA_SET_MARK_MASK]);
		else
			m->m = 0xffffffff;
	} else {
		m->v = m->m = 0;
	}
}