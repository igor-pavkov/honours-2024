static int stale_bundle(struct dst_entry *dst)
{
	return !xfrm_bundle_ok((struct xfrm_dst *)dst);
}