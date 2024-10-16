static void xfrm_bundle_flo_delete(struct flow_cache_object *flo)
{
	struct xfrm_dst *xdst = container_of(flo, struct xfrm_dst, flo);
	struct dst_entry *dst = &xdst->u.dst;

	/* Mark DST_OBSOLETE_DEAD to fail the next xfrm_dst_check() */
	dst->obsolete = DST_OBSOLETE_DEAD;
	dst_release_immediate(dst);
}