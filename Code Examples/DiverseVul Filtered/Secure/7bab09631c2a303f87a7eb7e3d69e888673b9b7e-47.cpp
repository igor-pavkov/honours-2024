static int xfrm_bundle_flo_check(struct flow_cache_object *flo)
{
	struct xfrm_dst *xdst = container_of(flo, struct xfrm_dst, flo);
	struct dst_entry *dst = &xdst->u.dst;

	if (!xdst->route)
		return 0;
	if (stale_bundle(dst))
		return 0;

	return 1;
}