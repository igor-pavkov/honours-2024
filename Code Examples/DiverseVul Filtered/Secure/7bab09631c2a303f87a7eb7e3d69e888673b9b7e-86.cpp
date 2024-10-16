static struct flow_cache_object *xfrm_bundle_flo_get(struct flow_cache_object *flo)
{
	struct xfrm_dst *xdst = container_of(flo, struct xfrm_dst, flo);
	struct dst_entry *dst = &xdst->u.dst;

	if (xdst->route == NULL) {
		/* Dummy bundle - if it has xfrms we were not
		 * able to build bundle as template resolution failed.
		 * It means we need to try again resolving. */
		if (xdst->num_xfrms > 0)
			return NULL;
	} else if (dst->flags & DST_XFRM_QUEUE) {
		return NULL;
	} else {
		/* Real bundle */
		if (stale_bundle(dst))
			return NULL;
	}

	dst_hold(dst);
	return flo;
}