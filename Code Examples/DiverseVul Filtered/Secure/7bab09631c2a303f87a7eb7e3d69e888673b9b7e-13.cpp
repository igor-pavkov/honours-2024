static int xfrm_policy_flo_check(struct flow_cache_object *flo)
{
	struct xfrm_policy *pol = container_of(flo, struct xfrm_policy, flo);

	return !pol->walk.dead;
}