static int xfrm_get_tos(const struct flowi *fl, int family)
{
	const struct xfrm_policy_afinfo *afinfo;
	int tos = 0;

	afinfo = xfrm_policy_get_afinfo(family);
	tos = afinfo ? afinfo->get_tos(fl) : 0;

	rcu_read_unlock();

	return tos;
}