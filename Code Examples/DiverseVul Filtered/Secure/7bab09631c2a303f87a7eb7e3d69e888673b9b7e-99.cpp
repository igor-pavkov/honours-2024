static struct flow_cache_object *xfrm_policy_flo_get(struct flow_cache_object *flo)
{
	struct xfrm_policy *pol = container_of(flo, struct xfrm_policy, flo);

	if (unlikely(pol->walk.dead))
		flo = NULL;
	else
		xfrm_pol_hold(pol);

	return flo;
}