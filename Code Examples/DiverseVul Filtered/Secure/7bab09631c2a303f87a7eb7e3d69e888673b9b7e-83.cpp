static const void *xfrm_get_dst_nexthop(const struct dst_entry *dst,
					const void *daddr)
{
	const struct dst_entry *path = dst->path;

	for (; dst != path; dst = dst->child) {
		const struct xfrm_state *xfrm = dst->xfrm;

		if (xfrm->props.mode == XFRM_MODE_TRANSPORT)
			continue;
		if (xfrm->type->flags & XFRM_TYPE_REMOTE_COADDR)
			daddr = xfrm->coaddr;
		else if (!(xfrm->type->flags & XFRM_TYPE_LOCAL_COADDR))
			daddr = &xfrm->id.daddr;
	}
	return daddr;
}