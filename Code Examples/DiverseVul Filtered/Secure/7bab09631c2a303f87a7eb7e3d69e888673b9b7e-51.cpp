static void xfrm_policy_flo_delete(struct flow_cache_object *flo)
{
	xfrm_pol_put(container_of(flo, struct xfrm_policy, flo));
}